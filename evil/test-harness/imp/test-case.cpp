#pragma once

#include "../test-case.h"

#include <string>
#include <functional>
#include <chrono>

namespace evil {

TestCase::TestCase(TestHarness *th,const char *name,type_testfunc func)
	:mpHarness{th}, mTestFunction(func), mName(name) {
	mbRun = false;
	mbPass = false;
	mmMilliSecs= ms::zero();
}

bool TestCase::run() {

	mbRun = true;
	auto t0 = Time::now();

	mbPass = mTestFunction(mpHarness,mMessage);

	auto t1 = Time::now();
	fsec delta = t1 - t0;

	mmMilliSecs = std::chrono::duration_cast<ms>(delta);

	return mbPass;
}

}