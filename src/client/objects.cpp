#include "objects.hpp"

#include <array>

#include "scene/splashscene.hpp"
#include "scene/titlescene.hpp"

template<typename ...P>
static consteval auto createObjectTable()
{ return std::array<const ObjectProfile*, sizeof...(P)>{{ &P::profile... }}; }

static constexpr auto ObjectProfilesA =
	createObjectTable<
		SplashScene,
		TitleScene
	>();

namespace Objects
{
	const ObjectProfile* const* profiles = ObjectProfilesA.data();
}
