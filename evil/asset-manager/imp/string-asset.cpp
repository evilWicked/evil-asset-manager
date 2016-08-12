#pragma once

#include "string-asset.h"
#include "../asset-manager.h"

namespace evil {
	
void StringAsset::load(const std::string &fname,const JSON &json) {
			
	for(JSON::const_iterator it = json.begin(); it != json.end(); ++it) {
		std::string strKey = it.key();
		std::string strVal = it.value().get<std::string>();
		evil::asset_manager().addAsset(new StringAsset(fname,strKey.c_str(), strVal.c_str()));
	}		
}

}