#pragma once

#include "network/packet.hpp"

#include <string>

class PKT_S2C_PongStart : public Network::Packet
{
public:
	DECL_PACKET()

	PKT_S2C_PongStart(u32 player1, u32 player2);

	explicit PKT_S2C_PongStart(Network::PacketBuffer& buf);
	void serialize(Network::PacketBuffer& buf) const override;

	[[nodiscard]] constexpr u32 getPlayer1() const { return m_player1; }
	[[nodiscard]] constexpr u32 getPlayer2() const { return m_player2; }

private:
	u32 m_player1;
	u32 m_player2;
};
