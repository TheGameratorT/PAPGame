#pragma once

#include "minigametype.hpp"
#include "network/packet.hpp"

#include <string>

class PKT_S2C_StartGame : public Network::Packet
{
public:
	DECL_PACKET()

	explicit PKT_S2C_StartGame(MiniGameType gameType);

	explicit PKT_S2C_StartGame(Network::PacketBuffer& buf);
	void serialize(Network::PacketBuffer& buf) const override;

	[[nodiscard]] constexpr MiniGameType getGameType() const { return m_gameType; }

private:
	MiniGameType m_gameType;
};
