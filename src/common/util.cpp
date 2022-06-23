#include "util.hpp"

namespace Util {

std::string utf8AsString(const U8String& str)
{
	SizeT strSize = str.rawSize();
	std::string out(strSize, '\0');
	std::memcpy(out.data(), str.data(), strSize);
	return out;
}

U8String stringAsUtf8(const std::string& str)
{
	SizeT strSize = str.size();
	std::basic_string<char8_t> strData(strSize, 0);
	std::memcpy(strData.data(), str.data(), strSize);
	U8String out;
	out.assign(strData);
	return out;
}

}
