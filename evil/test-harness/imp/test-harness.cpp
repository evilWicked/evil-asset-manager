#pragma once

#include "../test-harness.h"

namespace evil {
	
TestHarness::TestHarness(	const char *name, const char *log_name /*= "TEST_LOG.TXT"*/, 
							bool stopOnError /*= false*/, bool useStdOut /*= true*/)
							:mName(name),mLog(log_name), mbStopOnError(mbStopOnError), 
							mbUseStdOut(mbUseStdOut), mResultTable(6) {

	maColWidths.push_back(5);
	maColWidths.push_back(29);	
	maColWidths.push_back(5);
	maColWidths.push_back(6);
	maColWidths.push_back(5);
	maColWidths.push_back(29);
	muDotCount = 0;

}
	
TestHarness::~TestHarness() {
	for (auto it = mlistTests.begin(); it != mlistTests.end(); ++it) {
		delete(*it);
	};
}

void TestHarness::formatResults() {

	unsigned int passnum = 0;
	unsigned int runnum = 0;
	unsigned int total = (unsigned int)mlistTests.size();

	//count the results
	for (auto it = mlistTests.begin(); it != mlistTests.end(); ++it) {
		if ((*it)->mbRun)runnum++;
		if ((*it)->mbPass)passnum++;
	}

	//first row
	mResultTable.insertRow();
	mResultTable.insertCell("Tot:");
	mResultTable.insertCell(std::to_string(total));
	mResultTable.insertCell("Run:");
	mResultTable.insertCell(std::to_string(runnum));
	mResultTable.insertCell("Pass:");
	mResultTable.insertCell(std::to_string(passnum));

	mResultTable.insertRow();
	mResultTable.nextRow();

	mResultTable.insertCell("No.");
	mResultTable.insertCell("Name");
	mResultTable.insertCell("Run");
	mResultTable.insertCell("State");
	mResultTable.insertCell("ms");
	mResultTable.insertCell("Failed On");

	unsigned int i = 0;
	for (auto it = mlistTests.begin(); it != mlistTests.end(); ++it) {
		mResultTable.insertRow();
		mResultTable.nextRow();

		i++;
		mResultTable.insertCell(std::to_string(i));
		mResultTable.insertCell((*it)->mName);
		mResultTable.insertCell((*it)->mbRun ? "YES" : "NO");
		mResultTable.insertCell((*it)->mbPass ? "PASS" : "FAIL");		
		mResultTable.insertCell(std::to_string((*it)->mmMilliSecs.count()));
		mResultTable.insertCell((*it)->mMessage);
	}

	adjustCols();
}


void TestHarness::adjustCols() {

	int rows = mResultTable.Rows();
	int cols = mResultTable.Cols();
	std::string str;

	for (int j = 0; j < cols; ++j) {
		int colw = maColWidths[j];

		for (int i = 0; i < rows; ++i) {

			str = mResultTable.get(i, j);
			str.resize(colw, ' ');
			mResultTable.set(i, j, str);
		}
	}
}

void TestHarness::dumpLog() {
	std::ofstream log;
	log.open(mLog);

	int rows = mResultTable.Rows();
	int cols = mResultTable.Cols();

	log << std::endl;
	log << mName;
	log << std::endl;

	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			log << mResultTable.get(i, j);
		}
		log << std::endl;
	}
	log.close();
}

void TestHarness::dumpConsole() {

	int rows = mResultTable.Rows();
	int cols = mResultTable.Cols();

	std::cout << std::endl;
	std::cout << mName;
	std::cout << std::endl;

	for (int i = 0; i < rows; ++i) {
		for (int j = 0; j < cols; ++j) {
			std::cout << mResultTable.get(i, j);
		}
		std::cout << std::endl;
	}
}

void TestHarness::dump() {

	if (!mLog.empty()) {
		dumpLog();
	}

	if (mbUseStdOut) {
		dumpConsole();
	}
}
	
void TestHarness::add(const char *name, TestCase::type_testfunc func) {

	mlistTests.push_back(new TestCase(this,name,func));
}

void TestHarness::outDot() {
	
	if (mbUseStdOut) {
		std::cout << '.';
		if(muDotCount==79){
			muDotCount=0;
			std::cout << std::endl ;
		}
  }
}

void TestHarness::run() {

	test_list_iter it;
	int count=0;

	for (it = mlistTests.begin(); it != mlistTests.end(); ++it) {

		count++;

		outDot();

		if (!(*it)->run()) {
			if (mbStopOnError) break;
		}
	}

	formatResults();
	dump();
}

}