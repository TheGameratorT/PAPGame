#include "objects.hpp"

#include <array>

#include "scene/splashscene.hpp"
#include "scene/titlescene.hpp"
#include "scene/lobbyscene.hpp"
#include "scene/mgpongscene.hpp"
#include "scene/mgrythmscene.hpp"
#include "scene/mgtypewritescene.hpp"
#include "scene/resultsscene.hpp"

template<typename ...P>
static consteval auto createObjectTable()
{ return std::array<const ObjectProfile*, sizeof...(P)>{{ &P::profile... }}; }

static constexpr auto ObjectProfilesA =
	createObjectTable<
		SplashScene,
		TitleScene,
		LobbyScene,
		MGPongScene,
		MGRythmScene,
		MGTypeWriteScene,
		ResultsScene
	>();

namespace Objects
{
	const ObjectProfile* const* profiles = ObjectProfilesA.data();
}
