#pragma once

#include <stdexcept>
#include <string>
#include <cassert>
#include <iostream>

#ifdef EVIL_DEBUG

#define EVIL_ASSERT(condition,message) \
  { \
	  if(!(condition)) \
	  { \
		  std::cerr << "Assertion failed at " << __FILE__ << ":" << __LINE__; \
		  std::cerr << "Message" << #message; \
		  std::cerr << " inside " << __FUNCTION__ << std::endl; \
		  std::cerr << "Condition: " << #condition; \
		  abort(); \
	  } \
  } 
#else
#define EVIL_ASSERT(condition,message)
#endif


namespace evil {

	/** @brief An error exception

		Errors are generally considered to be fatal - such as json that wont parse at all rather than
		unknown tokens.
	*/
	class error : public std::runtime_error
	{
	public:
		error(const std::string& msg)
			:std::runtime_error(msg) {}

		error(const std::string& token, const std::string& msg)
			:std::runtime_error("\'" + token + "\' :" + msg) {}

		error(int token, const std::string& msg)
			:std::runtime_error("code " + std::to_string(token) + " :" + msg) {}
	};

	/** @brief  A warning exception.

	Warnings are errors that in themselves may be recoverable. This is
	something like a misspelt token in a json file in which the program 
	may be able to continue.

	(and no misspelt is not misspelt - unless you are a yank in which case 
	misspelt is officially misspelt as misspelled which sounds like something 
	a drunk wizard would do)
	
	*/
	class warning : public std::runtime_error
	{
	public:
		warning(const std::string& msg)
			:std::runtime_error(msg) {}

		warning(const std::string& token, const std::string& msg)
			:std::runtime_error("\'" + token + "\' :" + msg) {}

		warning(int token, const std::string& msg)
			:std::runtime_error("code " + std::to_string(token) + " :" + msg) {}
	};

	
}