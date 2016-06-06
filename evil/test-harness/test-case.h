#pragma once

#include <string>
#include <functional>
#include <chrono>
#include <list>
#include <fstream>
#include <vector>

#include "test-harness.h"
//implementation bought in at the bottom of the file

namespace evil {

class TestHarness;//forward declaration

///@brief provides a wrapper for a single test case, capturing the time it took to run.
class TestCase {

	typedef std::chrono::high_resolution_clock Time;
	typedef std::chrono::milliseconds ms;
	typedef std::chrono::duration<float> fsec;

public:
	typedef std::function<bool(TestHarness *th,std::string&)> type_testfunc;

	const std::string mName;
	std::string mMessage;
	bool mbPass;
	bool mbRun;
	ms mmMilliSecs;
	TestHarness *mpHarness;

	const type_testfunc mTestFunction;

	TestCase(TestHarness *th,const char *name,type_testfunc func);

	bool run();

public:
	//Hide and disable/default all constructors and destructors unless specifically overridden
	//this is a mnemonic to force me to think about things. if overridden they are commented
	//TestCase() = default;
	~TestCase() = default;
	TestCase(const TestCase& rhs) = delete;
	TestCase& operator=(const TestCase& rhs) = delete;
	TestCase(TestCase&& other) = delete;
	TestCase& operator=(TestCase&& other) = delete;
};

}

