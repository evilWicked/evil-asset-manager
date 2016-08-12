#pragma once

#include <string>
#include <vector>
#include<unordered_map>

#include <stdexcept>
#include <queue>
#include <functional>

#include "../../util/error.h"
#include "../../thread/thread-pool.h"

#include "imp/asset.h"
#include "imp/asset-loader.h"
#include "imp/asset-class.h"
#include"../asset-manager.h"

namespace evil {

AssetManager::AssetManager() :mstrAssetRoot{ "" }, muNumLoading{ 0 }{
			
	//we need to create an asset class to hold the asset files - am loading them them using
	//the threadpool the same as I would a pile of bitmaps.

	auto it = mapAssetClasses.find(EVIL_ASSET_DEF_CLASS_NAME);
	if(it != mapAssetClasses.end()) {
		mapAssetClasses.erase(it);
	}


	//emplace has trouble with char * so we put it into a string first
	std::string str= EVIL_ASSET_DEF_CLASS_NAME;
	auto p = mapAssetClasses.emplace(EVIL_ASSET_DEF_CLASS_NAME, str);
	//don't know why you would do it but allow overwrites of an asset definition just in case someone 
	//loads the same asset twice - whether the assets in that file allow overwriting is a different story
	(*p.first).second.setWarnExisting(false); 
	(*p.first).second.setReplaceExisting(true);

};

AssetManager::~AssetManager(){

};


AssetManager::loaderfn* AssetManager::getLoader(const std::string& key) {

	ReadWriteLock lock(&mRWLoadMutex, ReadWriteLock::READ);
	auto it = mapAssetLoaders.find(key);

	if(it != mapAssetLoaders.end()) {
		return &(*it).second;
	}else{
		return NULL;
	}
}


AssetClass& AssetManager::getAssetClass(const std::string& type) {

	{   //scope for the lock
		ReadWriteLock lock(&mRWClassMutex, ReadWriteLock::READ);
		auto it = mapAssetClasses.find(type);
		if(it != mapAssetClasses.end()) {
			return (*it).second;
		} else {
			throw error("unknown asset class:" + type);
		}
	}
};



void  AssetManager::addAsset(Asset *asset) {

	{//scope for the lock
		ReadWriteLock lock(&mRWClassMutex, ReadWriteLock::READ);
		auto it = mapAssetClasses.find(asset->mcstrType);
		if(it != mapAssetClasses.end()) {
			(*it).second.add(asset);
		} else {
			throw error("unknown asset class:" + asset->mcstrType);
		}
	}
};




void AssetManager::setAssetRoot(const std::string& root) {
	mstrAssetRoot = root;
};

std::string AssetManager::assetRoot() {
	return mstrAssetRoot; 
};


void AssetManager::addError(const error& e) {
	std::unique_lock<std::mutex> lock(mErrorMutex);
	mvecErrors.emplace_back(e.what());
};

void AssetManager::addWarning(const warning& w) {
	std::unique_lock<std::mutex> lock(mErrorMutex);
	mvecWarnings.emplace_back(w.what());
};

void AssetManager::setReplaceExisting(const std::string& class_key, bool bReplace) {

	{//scope for the lock
		ReadWriteLock lock(&mRWClassMutex, ReadWriteLock::READ);
		auto it = mapAssetClasses.find(class_key);
		if(it != mapAssetClasses.end()) {
			(*it).second.setReplaceExisting(bReplace);
		} else {
			throw error("unknown asset class:" + class_key);
		}
	}
}

void AssetManager::setWarnExisting(const std::string& class_key, bool bWarn) {
	{//scope for the lock
		ReadWriteLock lock(&mRWClassMutex, ReadWriteLock::READ);
		auto it = mapAssetClasses.find(class_key);
		if(it != mapAssetClasses.end()) {
			(*it).second.setWarnExisting(bWarn);
		} else {
			throw error("unknown asset class:" + class_key);
		}
	}
}

void AssetManager::registerAssetClass(const std::string& key, loaderfn&& func) {
	//we will allow asset classes to be overwritten so first look to see if we already
	//have a registered handler.  If so remove it and the associated asset class.
	
	{	//scope for the lock
		ReadWriteLock lock(&mRWLoadMutex,ReadWriteLock::WRITE);
		auto it = mapAssetLoaders.find(key);
		if(it != mapAssetLoaders.end()) {
			mapAssetLoaders.erase(it);
		}
		mapAssetLoaders[key] = func;
	}

	{//scope for the lock
		ReadWriteLock lock(&mRWClassMutex, ReadWriteLock::WRITE);
		auto it = mapAssetClasses.find(key);
		if(it != mapAssetClasses.end()) {
			mapAssetClasses.erase(it);		
		}
		mapAssetClasses.emplace(key, key);
	}
};


void AssetManager::queueAssetFile(const std::string& fpath) {

	AssetClass& aclass=getAssetClass(EVIL_ASSET_DEF_CLASS_NAME);

	Asset *asset = (Asset *)new AssetLoader(fpath);
	aclass.add(asset);

	{   //scope for the lock
		std::lock_guard<std::mutex> lock(mQueueMutex);		
		muNumLoading++;
		mqueueLoad.push(asset);	
	}	
};


void  AssetManager::deleteAssets(const std::string& cstr) {
};


void  AssetManager::releaseAssets(const std::string& cstr) {
};

void  AssetManager::startAssetLoad(Asset *asset) {
	//do nothing at the moment
};

void  AssetManager::endAssetLoad(Asset *asset) {
	std::lock_guard<std::mutex> lock(mQueueMutex);
	muNumLoading--;
	mcvLoadWait.notify_one();
};


void  AssetManager::loadAssets() {
	
	std::vector<ThreadPool::TaskFuture<Asset *>> vecFutures;

	while(muNumLoading) {

		//for each asset on the load queue pop it off and push the load function 
		//to the thread queue. This may result in other assets being pushed to 
		//the load queue as json is parsed.
		
		{  
			//scope for the lock
			std::unique_lock<std::mutex> lock(mQueueMutex);
			while(mqueueLoad.size()) {
				Asset *asset = mqueueLoad.front();
				mqueueLoad.pop();
				if(asset != NULL) {
					vecFutures.push_back(
						thread_pool::run(asset, &Asset::thread_load)
					);
				}
			}
			
			//wait on a notify from any thread. If muNumLoading > 0 then
			//this will loop around kicking of new loads and will wait again
			if(muNumLoading > 0) {
				mcvLoadWait.wait(lock);
			}
		}	
	}
	
	//wait here for all the threads to return their futures.
	for(auto& future : vecFutures) {
		Asset *assPtr=future.get(); //mmm unfortunate choice of label perhaps...
		//for every future trigger the asset loaded signal
		if(assPtr->hasLoaded()) {
			msigOnAssetLoad.dispatch(assPtr);
		}
	}

	if(mvecErrors.size()) {
		std::string msg;
		if(mvecErrors.size() == 1) {
			msg = std::to_string(mvecErrors.size()) + " Error found";
		} else {
			msg = std::to_string(mvecErrors.size()) + " Errors found";
		}
		msg +=" See  evil::asset_manager()::mvecErrors for details.";

		throw error(msg);

	} else if(mvecWarnings.size()) {
		std::string msg;
		if(mvecWarnings.size() == 1) {
			msg = std::to_string(mvecWarnings.size()) + " Warning found";
		} else {
			msg = std::to_string(mvecWarnings.size()) + " Warnings found";
		}
		msg += " See  evil::asset_manager()::mvecWarnings for details.";

		throw warning(msg);
	} 

	{
		//no errors? okay then lets dispatch the all assets loaded signal
		msigOnAllAssetsLoaded.dispatch();
	}
	
};




void AssetManager::deleteAssetClass(const std::string& key){

	{	//scope for the lock
		ReadWriteLock lock(&mRWLoadMutex, ReadWriteLock::WRITE);
		auto it = mapAssetLoaders.find(key);
		if(it != mapAssetLoaders.end()) {
			mapAssetLoaders.erase(it);
		}
	}

	{//scope for the lock
		ReadWriteLock lock(&mRWClassMutex, ReadWriteLock::WRITE);
		auto it = mapAssetClasses.find(key);
		if(it != mapAssetClasses.end()) {
			mapAssetClasses.erase(it);
		}
	}
};


}

