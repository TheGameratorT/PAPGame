#include "util.hpp"

namespace Util {

static std::string utf8AsString(const U8String& str)
{
	std::string out;
	out = reinterpret_cast<const char*>(str.data());
	return out;
}

}
