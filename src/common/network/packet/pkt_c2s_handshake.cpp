#include "pkt_c2s_handshake.hpp"

IMPL_PACKET(PKT_C2S_Handshake)

PKT_C2S_Handshake::PKT_C2S_Handshake(std::string magic, int gameVersion) :
	m_magic(std::move(magic)), m_gameVersion(gameVersion)
{}

PKT_C2S_Handshake::PKT_C2S_Handshake(Network::PacketBuffer& buf) :
	m_magic(buf.readString()),
	m_gameVersion(buf.read<int>())
{}

void PKT_C2S_Handshake::serialize(Network::PacketBuffer& buf) const
{
	buf.writeString(m_magic);
	buf.write<int>(m_gameVersion);
}
