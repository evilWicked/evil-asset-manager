#pragma once

#include <string>
#include <unordered_map>
#include <mutex>

#include "../../util/error.h"
#include "../../thread/read-write-mutex.h"
#include "../../thread/read-write-lock.h"

#include "asset.h"

namespace evil {
	
	/** @brief The Asset Class is used to group together assets of a given type.

		This class is used internally by the asset manager and is is normally not accessed directly.
			
		Internally it wraps up an unordered_map with some additional protection to stop
		unintended overwrite of existing assets. To avoid the high probability of assets with duplicate 
		names occuring during development it will throw an exception on an attempt to add an asset that
		already exists.  To intentionally replace an asset use the replace() function. 

		This class is thread safe and is protected with a read write lock allowing parallel reads
	*/
	class AssetClass {
		const std::string mstrName;

		ReadWriteMutex mRWMutex;		
		std::unordered_map<std::string, Asset *> mmapAssets;

		typedef std::unordered_map<std::string, Asset *>::iterator type_asset_iter;

		//boolean that allows the overwrite of existing elements
		bool mbReplaceExisting;

		//boolean that causes a warning to be generated on overwrite of existing elements
		bool mbWarnExisting;

	public:

		/** @brief Construct an asset class

			@param name of this asset class. This should be the key used by
			the asset manager to access this object.
		*/
		AssetClass(const std::string& name):mstrName{name}, 
			mbReplaceExisting{ false }, mbWarnExisting{ false } {
		}

		~AssetClass() {
			//on destruction delete all assets
			for(auto& x : mmapAssets) {
				delete x.second;
			}
			mmapAssets.clear();
		}

		void setReplaceExisting(bool bReplace) {
			mbReplaceExisting = bReplace;
		}

		void setWarnExisting(bool bWarn) {
			mbWarnExisting = bWarn;
		}

		/** @brief retrieve an asset, 
			
			returns null if the asset does not exist.

			@param key the key of the asset to retrieve
		*/
		Asset* get(const std::string& key) {
			ReadWriteLock lock(&mRWMutex, ReadWriteLock::READ);
			auto it = mmapAssets.find(key);
			if(it == mmapAssets.end()) {
				return NULL;
			}
			return mmapAssets[key]; 
		};

		/** @brief add an asset to this asset manager

			This will replace the asset if this asset class has been set to allow replacements.
			It will also warn on replacements if it has been set to do so. 

			@param key the key of the asset to retrieve
			@throw warning Throws an exception if the asset key already exists and the asset
			class has been set to warn on collsions with existing keys.
		*/
		void add(Asset* asset) {
			ReadWriteLock lock(&mRWMutex, ReadWriteLock::WRITE);
				
			auto it = mmapAssets.find(asset->mcstrKey);
			if(it != mmapAssets.end()) {
				//the asset already exists
				if(mbReplaceExisting) {
					//if we can replace existing then remove the existing element
					delete (*it).second;
					mmapAssets.erase(it);

					//insert the new one
					mmapAssets[asset->mcstrKey] = asset;

					if(mbWarnExisting) {					
						throw warning(asset->mcstrKey, "asset already existed - has been replaced");
					}

				} else {

					if(mbWarnExisting) {
						throw warning(asset->mcstrKey, "asset already exists - has not been replaced");
					}
				}			
					
			} else {
				//doesnt exist so insert it
				mmapAssets[asset->mcstrKey] = asset;
			}

				
		};

		/** @brief remove and destroy the asset indexed by key 
			   
			@param key The key of the asset to remove.
		*/
		void remove(const std::string& key) {
			ReadWriteLock lock(&mRWMutex, ReadWriteLock::WRITE);

			auto it = mmapAssets.find(key);
			if(it != mmapAssets.end()) {
				delete (*it).second;
				mmapAssets.erase(it);
			}
		};

		
		/** @brief check that they key exists

			@param key The key of the asset to check
		*/
		bool exists(const std::string& key) {
			ReadWriteLock lock(&mRWMutex, ReadWriteLock::WRITE);
			auto it = mmapAssets.find(key);
			return (it != mmapAssets.end());
		}


		/** @brief get the name of this class
		*/
		const std::string&  Name() { return mstrName; };


		//Hide and disable/default all constructors and destructors unless specifically overridden
		//AssetClass() = default;
		//~AssetClass() = default;
		AssetClass(const AssetClass& rhs) = delete;
		AssetClass& operator=(const AssetClass& rhs) = delete;
		AssetClass(AssetClass&& other) = delete;
		AssetClass& operator=(AssetClass&& other) = delete;
	};
	
}