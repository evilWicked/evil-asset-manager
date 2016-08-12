#pragma once 

#include "asset.h"
#include "../asset-manager.h"

namespace evil {
	
	/**@brief The Asset type that is used to manage strings.
		
		Pretty basic - it contains a string.
	*/
	class StringAsset :public Asset {
		const std::string mstrString;
	public:
		/** @brief Construct a String Asset.  
		*/
		StringAsset(const std::string& key, const std::string& str, const std::string& file):
					Asset(EVIL_STRING_ASSET_CLASS_NAME,key, file),mstrString{str}{
			mbPreload=true;
		}
		~StringAsset() {
		}
		/** @brief load - not used - no file to load.  
		*/
		const std::string& get(){return mstrString;}
			
		/** @brief load - not used - no file to load.  
		*/
		virtual void srcload(){};//does nothing in this class

		//Hide and disable/default all constructors and destructors unless specifically overridden
		//StringAsset() = default;
		//~StringAsset() = default;
		StringAsset(const StringAsset& rhs) = delete;
		StringAsset& operator=(const StringAsset& rhs) = delete;
		StringAsset(StringAsset&& other) = delete;
		StringAsset& operator=(StringAsset&& other) = delete;

		/** static factory method builds an asset and adds it to the asset manager 
			@param fname the name of the file that defined the asset (used for grouping and removal)
			@param json the json definition of the asset
		*/
		static void load(const std::string &fname,const JSON &json);
	};
	
}