#pragma once

#include "network/packet.hpp"
#include "math/vector.hpp"
#include "pongenums.hpp"

class PKT_S2C_PongMove : public Network::Packet
{
public:
	DECL_PACKET()

	PKT_S2C_PongMove(PongMovedObject movedObject, const Vec2i& position, PongMoveDirection direction);

	explicit PKT_S2C_PongMove(Network::PacketBuffer& buf);
	void serialize(Network::PacketBuffer& buf) const override;

	[[nodiscard]] constexpr PongMovedObject getMovedObject() const { return m_movedObject; }
	[[nodiscard]] constexpr const Vec2i& getPosition() const { return m_position; }
	[[nodiscard]] constexpr PongMoveDirection getDirection() const { return m_direction; }

private:
	PongMovedObject m_movedObject;
	Vec2i m_position;
	PongMoveDirection m_direction;
};
