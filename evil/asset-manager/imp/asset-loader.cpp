#pragma once 

#include <fstream>

#include "asset-loader.h"

namespace evil {


AssetLoader::AssetLoader(const std::string& fname)
	:Asset(EVIL_ASSET_DEF_CLASS_NAME, fname, fname) {

	mbPreload = true;
}

void AssetLoader::srcload() {

	std::string path = asset_manager().assetRoot() + mcstrKey;
	std::ifstream ifs(path);

	if(ifs.is_open()) {

		try {

			JSON json(ifs);
			for(JSON::iterator it = json.begin(); it != json.end(); ++it) {

				AssetManager::loaderfn* loadFunc = asset_manager().getLoader(it.key());
				if(loadFunc == NULL) {
					throw warning(it.key(),"No loader registered under this key");
				} else {
					JSON j = it.value();
					(*loadFunc)(mcstrKey,j);
				}
			}

		//a whole pile of different exceptions can be thrown. Rethrow as exceptions or warnings
		//to be collected in the manager.
		}catch(const std::invalid_argument& e) {
			//likely to be a parsing error
			throw error(path, e.what());

		}catch(const std::bad_alloc& e) {
			//can be thrown by JSON
			throw error(path, e.what());

		}catch(const std::domain_error& e) {
			//can be thrown by JSON
			throw error(path, e.what());

		}catch(const std::out_of_range& e) {
			//can be thrown by JSON
			throw error(path, e.what());

		}catch(const error& e) {
			//any error we have thrown
			throw error(path, e.what());

		}catch(const warning& w) {
			//any warning we have thrown
			throw warning(path, w.what());
		}
		
	}else {

		throw error(path, "asset file did not open.");
	}
	
}

}