#include "pkt_s2c_pongpoint.hpp"

#include "network/packetbuffer.hpp"

IMPL_PACKET(PKT_S2C_PongPoint)

PKT_S2C_PongPoint::PKT_S2C_PongPoint(bool isFinal, u32 paddleID) :
	m_isFinal(isFinal), m_paddleID(paddleID)
{}

PKT_S2C_PongPoint::PKT_S2C_PongPoint(Network::PacketBuffer& buf)
{
	m_isFinal = buf.readBool();
	m_paddleID = buf.read<u8>();
}

void PKT_S2C_PongPoint::serialize(Network::PacketBuffer& buf) const
{
	buf.write(m_isFinal);
	buf.write(m_paddleID);
}
