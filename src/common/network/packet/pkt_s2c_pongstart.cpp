#include "pkt_s2c_pongstart.hpp"

#include "network/packetbuffer.hpp"

IMPL_PACKET(PKT_S2C_PongStart)

PKT_S2C_PongStart::PKT_S2C_PongStart(u32 player1, u32 player2) :
	m_player1(player1), m_player2(player2)
{}

PKT_S2C_PongStart::PKT_S2C_PongStart(Network::PacketBuffer& buf)
{
	m_player1 = buf.read<u32>();
	m_player2 = buf.read<u32>();
}

void PKT_S2C_PongStart::serialize(Network::PacketBuffer& buf) const
{
	buf.write<u32>(m_player1);
	buf.write<u32>(m_player2);
}
