#pragma once

#include "network/packet.hpp"

#include <string>

class PKT_C2S_TypeState : public Network::Packet
{
public:
	DECL_PACKET()

	explicit PKT_C2S_TypeState(u32 charsTyped);

	explicit PKT_C2S_TypeState(Network::PacketBuffer& buf);
	void serialize(Network::PacketBuffer& buf) const override;

	[[nodiscard]] constexpr u32 getCharsTyped() const { return m_charsTyped; }

private:
	u32 m_charsTyped;
};
