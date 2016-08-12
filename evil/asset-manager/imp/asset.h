#pragma once


#include <string>
#include "../../signals/signal.h"
#include "../asset-manager.h"

//bring in the json handler dependency
#include "../../deps/json.hpp"

#define EVIL_ASSET_DEF_CLASS_NAME "asset-file"
#define EVIL_STRING_ASSET_CLASS_NAME "string"


namespace evil{

	using JSON = nlohmann::json;

	class Manager; //forward declaration

	/**@brief Virtual base class for an asset.

		An asset is some resource such as a bitmap, an audio file, a string or even a definition 
		for a class factory that can be defined in a file and used elsewhere in the program.

		Encapsulates an actual asset that can be accessed via the asset manager.
		Maintains the state of the asset.
	*/
	class Asset {

	protected:
		
		bool mbIsLoading;
		bool mbHasLoaded;
		bool mbPreload; //flags the resource for immediate load - by default loads on demand

	public:
		const std::string mcstrType;
		const std::string mcstrKey;
		const std::string mcstrDefinitionFile;

		/** @brief Asset constructor

			@param type required - a string defining the type of this asset
			@param key required - a string defining the key that will be used to store this asset.
			@param file required - a string that describes the file this asset was defined in
		*/
		Asset(const std::string& type, const std::string& key,const std::string& fname);

		/** @brief Asset destructor

			this will trigger the msigOnDestroy signal. If you have something that needs to be informed
			on destruction create an evil::Slot and attach a callback. A this pointer will be passed to the 
			slot.
		*/
		virtual ~Asset();


		/** @brief virtual function to load an asset from some source.

		Intended to be fully defined in derived asset specific class. As implemented here it is simply an 
		example that shows the structure of the srcload function.

		NOTE: Not every asset loads from a seperate src file.  strings for example are created
		directly from the asset definition file. This function is used to load bitmaps and audio etc
	
		@throw error exception thrown on any errors encountered in file processing
		If you want to pass an exception from your own load routine back to the manager use this.
		The asset manager attempts to let the threads come to a clean completion on errors before
		throwing a new exception from the main thread. 

		*/
		virtual void srcload()=0;
		///true if this asset is currently loading
		bool isLoading();
		///true if this asset has loaded
		bool hasLoaded();
			
		/** @brief Wrapper called by a loading thread to call the actual load function

			Not intended to be called by users.  Ensures the startAssetLoad and endAssetLoad
			functions are called for every load. 
			
			@returns a this pointer to the asset so that the loaded asset can be picked up from its future 			 
		*/
		Asset* thread_load();

		/** @brief Signal that will be triggered on destruction of this asset

		Passes a this pointer of the destroyed asset to the slot. For derived asset classes you
		may need to cast to the derived type depending on your need.
		*/
		Signal<Asset *> msigOnDestroy;

		//Hide and disable/default all constructors and destructors unless specifically overridden
		//Asset() = default;
		//~Asset() = default;
		Asset(const Asset& rhs) = delete;
		Asset& operator=(const Asset& rhs) = delete;
		Asset(Asset&& other) = delete;
		Asset& operator=(Asset&& other) = delete;
	};
			
}