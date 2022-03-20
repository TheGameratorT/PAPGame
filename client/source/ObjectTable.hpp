#pragma once

#include <cstdint>

#define INVALID_OBJECT_ID 0xFFFFFFFF

class ObjectProfile;

namespace ObjectTable
{
	extern const ObjectProfile* const* profiles;
	extern std::uint_fast32_t profileCount;

	inline const ObjectProfile* getObjectByID(std::uint_fast32_t id)
	{ return profiles[id]; }

	std::uint_fast32_t getObjectID(const ObjectProfile* profile);
}
