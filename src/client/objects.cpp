#include "objects.hpp"

#include <array>

#include "scene/splashscene.hpp"
#include "scene/titlescene.hpp"
#include "scene/lobbyscene.hpp"
#include "scene/mgtypewritescene.hpp"

template<typename ...P>
static consteval auto createObjectTable()
{ return std::array<const ObjectProfile*, sizeof...(P)>{{ &P::profile... }}; }

static constexpr auto ObjectProfilesA =
	createObjectTable<
		SplashScene,
		TitleScene,
		LobbyScene,
		MGTypeWriteScene
	>();

namespace Objects
{
	const ObjectProfile* const* profiles = ObjectProfilesA.data();
}
