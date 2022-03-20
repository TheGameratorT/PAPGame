#include "ObjectTable.hpp"

#include <array>

#include "scene/TitleScene.hpp"

class ObjectProfile;

template<typename ...P>
static consteval auto createObjectTable()
{ return std::array<const ObjectProfile*, sizeof...(P) + 1>{{ &P::profile..., nullptr }}; }

static constexpr auto ObjectProfilesA =
createObjectTable<
	TitleScene
>();

namespace ObjectTable
{
	const ObjectProfile* const* profiles = ObjectProfilesA.data();
	std::uint_fast32_t profileCount = ObjectProfilesA.size() - 1;

	std::uint_fast32_t getObjectID(const ObjectProfile* profile)
	{
		const ObjectProfile* iprofile;
		std::uint_fast32_t i = 0;
		while ((iprofile = profiles[i]) != nullptr)
		{
			if (profile == iprofile)
				return i;
			i++;
		}
		return INVALID_OBJECT_ID;
	}
}
