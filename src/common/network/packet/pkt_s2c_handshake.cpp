#include "pkt_s2c_handshake.hpp"

#include "network/packetbuffer.hpp"

IMPL_PACKET(PKT_S2C_Handshake)

PKT_S2C_Handshake::PKT_S2C_Handshake(std::string magic) :
	m_magic(std::move(magic))
{}

PKT_S2C_Handshake::PKT_S2C_Handshake(Network::PacketBuffer& buf) :
	m_magic(buf.readString())
{}

void PKT_S2C_Handshake::serialize(Network::PacketBuffer& buf) const
{
	buf.writeString(m_magic);
}
