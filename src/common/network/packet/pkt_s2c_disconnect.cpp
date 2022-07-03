#include "pkt_s2c_disconnect.hpp"

#include "network/packetbuffer.hpp"

IMPL_PACKET(PKT_S2C_Disconnect)

PKT_S2C_Disconnect::PKT_S2C_Disconnect(std::string message) :
	m_message(std::move(message))
{}

PKT_S2C_Disconnect::PKT_S2C_Disconnect(Network::PacketBuffer& buf) :
	m_message(buf.readString())
{}

void PKT_S2C_Disconnect::serialize(Network::PacketBuffer& buf) const
{
	buf.writeString(m_message);
}
