#pragma once

#include <type_traits>

class ObjectProfile;
class SplashScene;
class TitleScene;

#define OBJECTS_DEFINE_ID(name, id) if (std::is_same_v<name, T>) return id;

namespace Objects
{
	extern const ObjectProfile* const* profiles;

	inline const ObjectProfile* getProfileByID(int id)
	{ return profiles[id]; }

	template<class T>
	constexpr int idOf()
	{
		OBJECTS_DEFINE_ID(SplashScene, 0)
		OBJECTS_DEFINE_ID(TitleScene, 1)
		return -1;
	}
}

#undef OBJECTS_DEFINE_ID