#include "pkt_s2c_pongmove.hpp"

#include "network/packetbuffer.hpp"

IMPL_PACKET(PKT_S2C_PongMove)

PKT_S2C_PongMove::PKT_S2C_PongMove(PongMovedObject movedObject, const Vec2i& position, PongMoveDirection direction) :
	m_movedObject(movedObject), m_position(position), m_direction(direction)
{}

PKT_S2C_PongMove::PKT_S2C_PongMove(Network::PacketBuffer& buf)
{
	m_movedObject = PongMovedObject(buf.read<u32>());
	m_position.x = buf.read<i32>();
	m_position.y = buf.read<i32>();
	m_direction = PongMoveDirection(buf.read<u32>());
}

void PKT_S2C_PongMove::serialize(Network::PacketBuffer& buf) const
{
	buf.write(u32(m_movedObject));
	buf.write<i32>(m_position.x);
	buf.write<i32>(m_position.y);
	buf.write(u32(m_direction));
}
