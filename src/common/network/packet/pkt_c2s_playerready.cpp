#include "pkt_c2s_playerready.hpp"

#include "network/packetbuffer.hpp"

IMPL_PACKET(PKT_C2S_PlayerReady)

PKT_C2S_PlayerReady::PKT_C2S_PlayerReady(ReadyType ready) :
	m_ready(ready)
{}

PKT_C2S_PlayerReady::PKT_C2S_PlayerReady(Network::PacketBuffer& buf) :
	m_ready(ReadyType(buf.read<u8>()))
{}

void PKT_C2S_PlayerReady::serialize(Network::PacketBuffer& buf) const
{
	buf.write<u8>(m_ready);
}
