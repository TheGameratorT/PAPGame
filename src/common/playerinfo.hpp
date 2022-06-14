#pragma once

#include <string>

#include "types.hpp"
#include "locale/unicodestring.hpp"

struct PlayerInfo
{
	u32 id;
	U8String name;
	u32 team;
};
