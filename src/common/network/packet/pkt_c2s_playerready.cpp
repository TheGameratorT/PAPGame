#include "pkt_c2s_playerready.hpp"

IMPL_PACKET(PKT_C2S_PlayerReady)

PKT_C2S_PlayerReady::PKT_C2S_PlayerReady(bool ready) :
	m_ready(ready)
{}

PKT_C2S_PlayerReady::PKT_C2S_PlayerReady(Network::PacketBuffer& buf) :
	m_ready(buf.readBool())
{}

void PKT_C2S_PlayerReady::serialize(Network::PacketBuffer& buf) const
{
	buf.writeBool(m_ready);
}
