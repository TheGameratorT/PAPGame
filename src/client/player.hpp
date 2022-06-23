#pragma once

#include "types.hpp"
#include "locale/unicodestring.hpp"
#include "playerinfo.hpp"

class Player
{
public:
	Player();
	explicit Player(const PlayerInfo& info);
	~Player();

	[[nodiscard]] constexpr u32 getID() const { return m_id; }
	[[nodiscard]] constexpr const U8String& getName() const { return m_name; }

	[[nodiscard]] constexpr bool getReady() const { return m_isReady; }
	constexpr void setReady(bool ready) { m_isReady = ready; }

	void onConnect();
	void onDisconnect();

private:
	u32 m_id;
	U8String m_name;
	bool m_isReady;
};
