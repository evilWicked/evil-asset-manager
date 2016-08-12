
#include <string>
#include <codecvt>
#include <locale>
#include "../utility.h"

namespace evil {

	namespace util{

		///generate an application unique id
		unsigned int uniqueId()
		{
			static std::atomic<unsigned int> uID{ 0 };
			return ++uID;
		}
		
		std::u16string strToUTF16(std::string str){

			std::wstring_convert<std::codecvt_utf8_utf16<int16_t>,int16_t> convert; 
			auto tmp=convert.from_bytes(str);
			const char16_t* u16=reinterpret_cast<const char16_t *>(tmp.data());
			return std::u16string(u16);
		}

		std::string UTF16toStr(std::u16string str16){

			std::wstring_convert<std::codecvt_utf8_utf16<int16_t>,int16_t> convert; 
			auto p = reinterpret_cast<const int16_t *>(str16.data());
			return convert.to_bytes(p, p + str16.size());
		}
	
	}

}