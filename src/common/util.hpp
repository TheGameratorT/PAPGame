#pragma once

#include <string>

#include "locale/unicodestring.hpp"

namespace Util {

std::string utf8AsString(const U8String& str);
U8String stringAsUtf8(const std::string& str);

}
