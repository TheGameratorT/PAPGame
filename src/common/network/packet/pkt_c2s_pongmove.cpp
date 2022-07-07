#include "pkt_c2s_pongmove.hpp"

#include "network/packetbuffer.hpp"

IMPL_PACKET(PKT_C2S_PongMove)

PKT_C2S_PongMove::PKT_C2S_PongMove(PongMoveDirection moveDirection) :
	m_moveDirection(moveDirection)
{}

PKT_C2S_PongMove::PKT_C2S_PongMove(Network::PacketBuffer& buf) :
	m_moveDirection(PongMoveDirection(buf.read<u32>()))
{}

void PKT_C2S_PongMove::serialize(Network::PacketBuffer& buf) const
{
	buf.write(u32(m_moveDirection));
}
