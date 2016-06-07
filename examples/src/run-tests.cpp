#pragma once

#ifdef _WIN32

#define EVIL_WIN

//visual leak detector. 
//If you enable this note that visual leak detector sometimes reports some false
//positives due to top level objects not being deleted after the leak detection code 
//#include <vld.h> 

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
#include "evil/loader/loader.h"

//bring in the wrapper which brings in the source
#include "evil/loader/evil-wrapper.cpp"



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

	evil::loader 
	
	#include "tests/tc-load-json.h"
					
	Tests.run();
	
	std::cout << "Press return to exit";
	std::getchar();

}
