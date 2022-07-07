#pragma once

#include "network/packet.hpp"

class PKT_S2C_PongPoint : public Network::Packet
{
public:
	DECL_PACKET()

	explicit PKT_S2C_PongPoint(bool isFinal, u32 paddleID);

	explicit PKT_S2C_PongPoint(Network::PacketBuffer& buf);
	void serialize(Network::PacketBuffer& buf) const override;

	[[nodiscard]] constexpr bool getIsFinal() const { return m_isFinal; }
	[[nodiscard]] constexpr u32 getPaddleID() const { return m_paddleID; }

private:
	bool m_isFinal;
	u8 m_paddleID;
};
