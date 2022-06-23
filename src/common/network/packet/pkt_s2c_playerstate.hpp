#pragma once

#include "network/packet.hpp"
#include "playerinfo.hpp"

#include <string>
#include <memory>

class PKT_S2C_PlayerState : public Network::Packet
{
public:
	DECL_PACKET()

	enum State
	{
		Join,
		Leave,
		Ready
	};

	PKT_S2C_PlayerState(u32 playerID, State state, bool ready);
	PKT_S2C_PlayerState(u32 playerID, State state, PlayerInfo* info);
	~PKT_S2C_PlayerState() override;

	explicit PKT_S2C_PlayerState(Network::PacketBuffer& buf);
	void serialize(Network::PacketBuffer& buf) const override;

	[[nodiscard]] constexpr u32 getPlayerID() const { return m_playerID; }
	[[nodiscard]] constexpr State getState() const { return m_state; }
	[[nodiscard]] constexpr const PlayerInfo& getPlayerInfo() const { return *m_playerInfo; }
	[[nodiscard]] constexpr bool getReady() const { return m_ready; }

private:
	u32 m_playerID;
	State m_state;
	PlayerInfo* m_playerInfo;
	bool m_ready;
};
