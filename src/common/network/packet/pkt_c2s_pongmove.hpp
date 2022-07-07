#pragma once

#include "network/packet.hpp"
#include "pongenums.hpp"

class PKT_C2S_PongMove : public Network::Packet
{
public:
	DECL_PACKET()

	explicit PKT_C2S_PongMove(PongMoveDirection moveDirection);

	explicit PKT_C2S_PongMove(Network::PacketBuffer& buf);
	void serialize(Network::PacketBuffer& buf) const override;

	[[nodiscard]] constexpr PongMoveDirection getMoveDirection() const { return m_moveDirection; }

private:
	PongMoveDirection m_moveDirection;
};
