#pragma once

#include <string>
#include <queue>
#include <functional>
#include <unordered_map>


#include "imp/asset.h"
#include "imp/asset-class.h"
#include "imp/asset-loader.h"
#include "imp/string-asset.h"

#include"../util/error.h"
#include"../signals/signal.h"


/** @brief The evil namespace.

	evil things will assume that other evil things know they are evil, so dont need to be told they are evil.
	Outside the evil namespace there is likely to be conflict between such things as signal so its best to avoid
	generic "using namespace evil" in favour of explicitly prefixing evil entities with evil::
*/
namespace evil {

	
	/** @brief The asset manager

		This provides the central point from which assets can be managed. There exists singleton access
		via evil::asset:manager to all resources.
	*/
	class AssetManager {
		//let the asset loader reach in and add resources
		friend AssetLoader;

		typedef std::function<void(const std::string &,JSON &)> loaderfn;
		//point this to the programs asset directory
		std::string mstrAssetRoot;

		//provide a read write mutex to access the assets themselves - allowing parallel reads
		ReadWriteMutex mRWClassMutex;
		std::unordered_map <std::string,AssetClass> mapAssetClasses;

		//we will need another mutex to protect the loader call backs - also read write
		ReadWriteMutex mRWLoadMutex;
		std::unordered_map <std::string, loaderfn> mapAssetLoaders;

		//provide a std mutex to protect the load queue 
		std::mutex mQueueMutex;
		std::queue<Asset *> mqueueLoad;	

		//a condition variable to enable us to wait during load
		//unless new jobs have been pushed to the load queue
		std::condition_variable mcvLoadWait;

		//keep track of how many are loading - not the number on the queue. We will decrement this when
		//the parsing of a file has completed - not when it is popped.
		unsigned int muNumLoading;
						
		//while we are going overboard with mutexes we also need one to protect the
		//error vectors.
		std::mutex mErrorMutex;

		//helper used by the asset loader to call the load function associated with an asset class
		AssetManager::loaderfn* getLoader(const std::string& key);

		/** @brief Get an asset class

		*/
		AssetClass& getAssetClass(const std::string& type);

	public:
			
		/** @brief evil::Signal that will be triggered when the last queued asset is complete

		To use this to trigger the next phase of a program boot sequence and to avoid issues of loads
		occuring faster than you have queued jobs consider queueing all asset files before calling load
		*/
		Signal<> msigOnAllAssetsLoaded;

		/** @brief evil::Signal that will be fired when the load of an asset is complete

		Passes a pointer to the asset to the slot. For derived asset classes you
		may need to cast to the derived type depending on your need.

		Needless to say this can't live on any specific asset because it doesn't yet exist
		so you will need a handler to watch the asset via the pointer and if its type and name matches
		something you are looking for then do something.

		You could refine this by adding a slot before loading the asset loader file that contains the asset 
		you are waiting for and removing it after the asset (not the loader file) has loaded.

		Remember that evil::Signal has an addOnce() function that is intended for this purpose. The attached
		slot will be removed from the signal after being triggered.
		*/
		Signal<Asset *> msigOnAssetLoad;

		/** @brief Constructs the asset class
		*/
		AssetManager();

		/** @brief Destructor
		*/
		~AssetManager();
			
		/** @brief Set a root directory for the asset loader

		@param root A const char * string that defines the root path for all asset loading.
		It is expected that this will be terminated with a /
		*/
		void setAssetRoot(const std::string& root);

		/** @brief The asset root being used by the file manager
		*/
		std::string assetRoot();

		
		/** @brief Register an asset type and handler

		Registers an asset class (derived from AssetDefinition) to handle specific types of assets.
		The key used for this needs to match the string used in the asset definition file.

		the default asset handlers are

		"string"
		"bitmap"
		"spritesheet"

		The default asset handlers can be overridden by registering a new loader against an existing
		key. The preferred way to do this is to derive a new asset class from Asset - see for example
		the libpng and freetype handler examples (these need you to include libraries). This will enable
		your asset class to be accessed via the manager.
			
		Alternatively if you want to bypass the asset manager altogether simply register your loader function.
		It is a std::function which can accept a lambda or a bind expression - let it hand you the json
		and you can do what you want with it.

		you can simply 

		todo expand on this

		*/
		void registerAssetClass(const std::string& key, loaderfn&& func);

		


		/** @brief Remove an asset class and associated handlers

		*/
		void deleteAssetClass(const std::string& key);

		/** @brief Allow the loader to replace assets with the same key within an asset class.

		The default behaviour is not to allow an asset to overwrite another asset of the the same type and name.
		Setting this to true will allow it.  To be warned on collision use setWarnExisting("classkey",true)

		Note this refers to assets within an asset class - not the asset class itself.
		Asset classes can always be replaced if you want to customize a handler.

		Assets in this context means for example two strings with the same key defined in two different files.
		You may intentionally be updating a string with another in a different language - internationalisation
		or or perhaps you missed something somewhere in a pile of files.

		Your choice.

		@param class_key the key to the class you want this applied to
		@param bReplace if true will replace an existing asset of the same type with the same key.

		*/
		void setReplaceExisting(const std::string& class_key, bool bReplace);

		/** @brief Generate a warning on asset key collision

		This will gnerate a warning exception warn if an asset key of a given class already exists.
		Whether it proceeds to overwrite the existing asset depends on how setReplaceExisting is set.

		The default behaviour is not to warn.  Turn it on during development if needed while you
		are playing around with things and turn it off when you are happy you have no unexpected collisions

		@param class_key the key to the class you want this applied to
		@param bReplace if true throw a warning exception
		*/
		void setWarnExisting(const std::string& class_key, bool bWarn);

		/** @brief Queue an asset file with the loader.

		@param fpath A const char * string that contains the path of the asset file with respect to 
		the asset root.

		This function is threadsafe. It is intended to be called from within the various threads that are
		processing the Assets on the load queue.
		*/
		void queueAssetFile(const std::string& fpath);

		/** @brief Encapsulates any activities required at the start of an asset load.

		Not intended to be called publicly - but needed here so that the asset class can reach it
		*/
		void startAssetLoad(Asset *asset);

		/** @brief Encapsulates any activities required at the end of an asset load.

		Not intended to be called publicly - but needed here so that the asset class can reach it
		*/
		void endAssetLoad(Asset *asset);


		/** @brief Start processing the asset queue

		This will load asset definition files on the load queue. It may push new assets onto the load
		queue as it processes which will also load if they have been flagged as preload=true. It will use
		multiple threads to do this. 

		It is intended that this be called from a single thread - most likely main thread.

		*/
		void loadAssets();

		/** @brief Deletes all assets defined in a given asset file

		Intended for such scenarios as a game where you have a pile of assets defined in a given 
		asset file that are no longer going to be used. This will unload and destroy the assets defined
		in that file - intended to reclaim the memory. Care that the asset is not being used.

		@param fpath A const char * string that contains the path of the asset file with respect to 
		the asset root.
		*/
		void deleteAssets(const std::string& fpath);

		/** @brief Release all assets defined in a given asset file

		This releases all assets that can be released. That is those with a src attribute. It does not delete
		the asset itself. If the asset is needed again simply use it. It will load on demand. It does not delete
		strings and definitions.  In simple terms it releases big hunks of data such images and audio data that
		it can easily reload again.

		@param fpath A const char * string that contains the path of the asset file with respect to
		the asset root.
		*/
		void releaseAssets(const std::string& fpath);

		/** @brief helper used by the asset loader to add an asset to an asset class

			@param asset a pointer to an asset derived from an asset class
		*/
		void addAsset(Asset *asset);


		/** @brief get an asset of a particular type

			@return  a const reference to the assets content
		*/
		template<typename T>
		const T& get(const std::string& type, const std::string& key) {
			return getAssetClass(type).get(key).get();
		};

		/** Collects errors thrown on the load threads.
			
		Because of the high probability of typos in JSON files and all the usual problems
		created by humans editing things meant to be read by machines errors are to be expected. 
		And to make it interesting these are occuring on different threads.

		Exceptions messages from the JSON load are captured here and a new exception thrown
		from the main thread to alert the program
		*/
		std::vector<std::string> mvecErrors;
		void addError(const error& e);
				
		/** Collects warnings thrown on the load threads.

			Like errors it is highly likely that these will occur. Generally these will be unknown tokens
			resulting from mispellings. I am treating them seperately because they may be surviviable
		*/
		std::vector<std::string> mvecWarnings;
		void addWarning(const warning& w);
			
		//Hide and disable/default all constructors and destructors unless specifically overridden
		//AssetManager() = default;
		//~AssetManager() = default;
		AssetManager(const AssetManager& rhs) = delete;
		AssetManager& operator=(const AssetManager& rhs) = delete;
		AssetManager(AssetManager&& other) = delete;
		AssetManager& operator=(AssetManager&& other) = delete;
	};


	/**
		namespace singleton access to the manager
	*/
	inline AssetManager& asset_manager(void) {
		static AssetManager defaultManager;
		return defaultManager;
	}

}

