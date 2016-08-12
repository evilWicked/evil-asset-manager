#pragma once

#include <string>
#include "../../util/error.h"
#include "../asset-manager.h"

namespace evil{
	
Asset::Asset(const std::string& type, const std::string& key,const std::string& fname)
		:mcstrType{ type }, mcstrKey{ key }, mbIsLoading{ false }, mbHasLoaded{ false }, 
	     mbPreload{ false }, mcstrDefinitionFile{ fname } {
}

Asset::~Asset(){
}

bool Asset::isLoading() { 
	return mbIsLoading; 
}

bool Asset::hasLoaded() {
	return mbHasLoaded; 
}


Asset* Asset::thread_load() {
	mbIsLoading = true;
	asset_manager().startAssetLoad(this);

	try {

		srcload();

	}catch(const error& e) {
		//on an exception terminate this load but let the others continue.
		//we will throw another exception from the main thread to cover all
		//errors found on all threads later.
		asset_manager().addError(e);
		mbIsLoading = false;
		mbHasLoaded = false;
		asset_manager().endAssetLoad(this);
		return this;

	} catch(const warning& w) {
		//on an exception terminate this load but let the others continue.
		//we will throw another exception from the main thread to cover all
		//errors found on all threads later.
		asset_manager().addWarning(w);
		mbIsLoading = false;
		mbHasLoaded = false;
		asset_manager().endAssetLoad(this);
		return this;
	}
	

	mbIsLoading = false;
	mbHasLoaded = true;
	asset_manager().endAssetLoad(this);
	return this;
}

}