#pragma once 

#include "asset.h"
#include "../asset-manager.h"

namespace evil {
	
	/**@brief The Asset type that is used to load other Assets.

		This asset type represents a JSON file that contains other asset definitions. It will be run
		on its own thread creating other Assets as it parses the keys in the JSON. Where the 
		new assets are flagged as preload and contain src references these Assets will also be pushed 
		onto the load queue to be picked up by the next spare thread.

		Where the assets are not preload and contain src they will load on demand.

		Where the asset does not require src they will be created immediately. 

		In practice what this means is that strings and factory definitions will load immediately
		bitmaps, spritesheets, texturemaps and audio will load immediately if flagged as preload
		otherwise they will load when you need them.

		A final option if you have something like a game with many levels is to load each levels 
		assets in their own asset file and mark them as preload.  But don't load the asset file 
		until the end of the previous level.
	*/
	class AssetLoader :public Asset {
			
	public:
		/** @brief Construct an Asset Loader Asset.  
		The key will be the file name
		*/
		AssetLoader(const std::string& fname);

		virtual void srcload();

		//Hide and disable/default all constructors and destructors unless specifically overridden
		//AssetLoader() = default;
		~AssetLoader() = default;
		AssetLoader(const AssetLoader& rhs) = delete;
		AssetLoader& operator=(const AssetLoader& rhs) = delete;
		AssetLoader(AssetLoader&& other) = delete;
		AssetLoader& operator=(AssetLoader&& other) = delete;
	};
}