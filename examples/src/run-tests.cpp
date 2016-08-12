#pragma once

#ifdef _WIN32

#define EVIL_WIN

//visual leak detector. 
//If you enable this note that visual leak detector sometimes reports some false
//positives due to top level objects not being deleted after the leak detection code 
#include <vld.h> 

#include <windows.h>

#elif __APPLE__
#define EVIL_OSX
#endif

#include <iostream>
#include <vector>
#include <string>
#include <unordered_set>
#include <queue>

//include the main header as you normally would
#include "evil/asset-manager/asset-manager.h"

//bring in the wrapper which brings in the source
#include "evil/asset-manager/evil-wrapper.cpp"

//You would normally not use these

//bring in the evil test harness 
#include "evil/test-harness/test-harness.h"

//bring in the test wrapper which includes the test harness implementation files
#include "evil/test-harness/evil-wrapper.cpp"


//this is an example so we wont assume the evil namespace - lets do it as a user would do it

int main(int argc, char* argv[])
{
	
	evil::TestHarness Tests("Evil Loader Testing");

	//include our test cases - they are just lambdas added by
	//cTests.add("some name", [](evil::CTestHarness *th,std::string& result)mutable->bool 
	//							{do something return pass/fail})

	evil::asset_manager().setAssetRoot("../test-data/");
	
	//#include "tests/tc-load-json.h"
			
	evil::asset_manager().registerAssetClass(EVIL_STRING_ASSET_CLASS_NAME, &evil::StringAsset::load);
	evil::asset_manager().setWarnExisting(EVIL_STRING_ASSET_CLASS_NAME,true);
	evil::asset_manager().setReplaceExisting(EVIL_STRING_ASSET_CLASS_NAME,true);

	
	evil::asset_manager().queueAssetFile("test-data-2-A.json");
	evil::asset_manager().queueAssetFile("test-data-2-B.json");
	evil::asset_manager().queueAssetFile("test-data-2-C.json");
	evil::asset_manager().queueAssetFile("test-data-2-D.json");
	evil::asset_manager().queueAssetFile("test-data-2-E.json");
	evil::asset_manager().queueAssetFile("test-data-2-F.json");
	evil::asset_manager().queueAssetFile("test-data-2-G.json");
	
	
	evil::Slot<evil::Asset *> asset_load_slot;

	asset_load_slot.attach([](evil::Asset *x) {
		std::cout <<"TYPE: "<< x->mcstrType <<" NAME: "<< x->mcstrKey << "\n";
	});
	evil::asset_manager().msigOnAssetLoad.add(&asset_load_slot);

	evil::Slot<> end_load_slot;

	end_load_slot.attach([]() {
		std::cout << "END LOAD CALLED \n";
	});
	evil::asset_manager().msigOnAllAssetsLoaded.add(&end_load_slot);

	

	try {

		evil::asset_manager().loadAssets();

	}catch(evil::error& e){
		std::cout << e.what() << "\n";

		for(auto& err : evil::asset_manager().mvecErrors) {
			std::cout << err << "\n";
		}

	}catch(evil::warning& w) {
		std::cout << w.what() << "\n";

		for(auto& warn : evil::asset_manager().mvecWarnings) {
			std::cout << warn << "\n";
		}
	}

	//evil::asset_manager().deleteAssetClass(EVIL_STRING_ASSET_CLASS_NAME);

	evil::AssetManager& mgr = evil::asset_manager();

	Tests.run();
	
	std::cout << "Press return to exit";
	std::getchar();

}
