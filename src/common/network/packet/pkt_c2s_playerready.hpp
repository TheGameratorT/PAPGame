#pragma once

#include "network/packet.hpp"

#include <string>

class PKT_C2S_PlayerReady : public Network::Packet
{
public:
	DECL_PACKET()

	explicit PKT_C2S_PlayerReady(bool ready);

	explicit PKT_C2S_PlayerReady(Network::PacketBuffer& buf);
	void serialize(Network::PacketBuffer& buf) const override;

	[[nodiscard]] constexpr bool getReady() const { return m_ready; }

private:
	bool m_ready;
};
