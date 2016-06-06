//this file simply contains the definition of a test case that will be included into 
//the run-tests.cpp file. I have simply copied it out to its own file which I am including back into
//the main program to clean things up a little. Dont worry about Visual Studio making it look like 
//it has errors. That is due to it not having any understanding of the context it runs in.


Tests.add("test", [](evil::TestHarness *th,std::string& result)mutable->bool {
	
	
		//got here so we passed
		return true;
});