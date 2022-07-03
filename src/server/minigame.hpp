#pragma once

#include "common/concepts.hpp"
#include "common/typetraits.hpp"

#define DECL_MINIGAME() \
static MiniGame* construct(); \
static const Profile profile; \
const Profile& getProfile() const override;

#define IMPL_MINIGAME(Derived, Name) \
MiniGame* Derived::construct() { return new Derived(); } \
const MiniGame::Profile Derived::profile = { construct, Name }; \
const MiniGame::Profile& Derived::getProfile() const { return profile; }

class MiniGame
{
public:
	using CtorT = MiniGame*(*)();

	struct Profile
	{
		CtorT ctor;
		const char* name;
	};

	virtual ~MiniGame();

	[[nodiscard]] virtual const Profile& getProfile() const = 0;

	virtual void onCreate();
	virtual void onUpdate();
	virtual void onDestroy();
	virtual void onClientsReady();

	template<class T>
	T* as() { return static_cast<T*>(this); }
};

namespace CC
{
	template<class T>
	concept BaseOfMiniGame = CC::BaseOf<MiniGame, T>;
}
