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
#include "evil/signals/loader.h"

//bring in the wrapper which brings in the source
#include "evil/signals/evil-wrapper.cpp"



//You would normally not use these

//bring in the evil test harness 
#include "evil/test-harness/test-harness.h"

//bring in the test wrapper which includes the test harness implementation files
#include "evil/test-harness/evil-wrapper.cpp"


//the worlds most absurd math library - contains a random integer generator. 
//Will probably extend it in the future
#include "evil/math/math.h"

//some test objects we will use to test things with

class TestClass {
	std::mutex mMtx;
public:
	int ival;
	double dval;
	std::string strval;

	TestClass() {
		ival = 0;
		dval = 0.0;
		strval = "";
	}

	void testFunction1(int i) { 
	//	std::lock_guard<std::mutex> lock(mMtx);
		ival += i; 
	}

	void testFunction2(double x) { 
		std::lock_guard<std::mutex> lock(mMtx);
		dval = x; 
	}

	void testFunction3(int i, double x,std::string &str) {
	//	std::lock_guard<std::mutex> lock(mMtx);
		ival = i;
		dval = x;
		strval = "got here";
	}

	void testFunction4(int i) {
		
		ival += i;
	//	std::vector<int> vecX;
		for(int i = 0; i < 20; i++) {
			//vecX.push_back(evil::math::rand_int(-100,100));
			dval += evil::math::rand_int(-100, 100);
		}
		//std::sort(vecX.begin(), vecX.end());
		//ival = vecX[19];
	}
};


int iTestVal=0;
double dTestVal=0.0;

void testFunction1(int i) {
	iTestVal += i;
}

void testFunction2(double x) {
	dTestVal = x;
}

void testFunction3(int i, double x, std::string &str) {
	iTestVal = i;
	dTestVal = x;
	str = "got here";
}



//this is an example so we wont assume the evil namespace - lets do it as a user would do it

int main(int argc, char* argv[])
{
	
	evil::TestHarness Tests("Evil Signals Testing");

	//include our test cases - they are just lambdas added by
	//cTests.add("some name", [](evil::CTestHarness *th,std::string& result)mutable->bool 
	//							{do something return pass/fail})

	
	#include "tests/tc-test.h"
					
	Tests.run();
	
	std::cout << "Press return to exit";
	std::getchar();

}
