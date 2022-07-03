#include "pkt_c2s_handshake.hpp"

#include "network/packetbuffer.hpp"

IMPL_PACKET(PKT_C2S_Handshake)

PKT_C2S_Handshake::PKT_C2S_Handshake(std::string magic, u32 gameVersion, U8String playerName) :
	m_magic(std::move(magic)), m_gameVersion(gameVersion), m_playerName(std::move(playerName))
{}

PKT_C2S_Handshake::PKT_C2S_Handshake(Network::PacketBuffer& buf) :
	m_magic(buf.readString()),
	m_gameVersion(buf.read<u32>()),
	m_playerName(buf.readU8String())
{}

void PKT_C2S_Handshake::serialize(Network::PacketBuffer& buf) const
{
	buf.writeString(m_magic);
	buf.write<u32>(m_gameVersion);
	buf.writeU8String(m_playerName);
}
