#include "pkt_c2s_lobbymsg.hpp"

#include "network/packetbuffer.hpp"

IMPL_PACKET(PKT_C2S_LobbyMsg)

PKT_C2S_LobbyMsg::PKT_C2S_LobbyMsg(U8String msg) :
	m_msg(std::move(msg))
{}

PKT_C2S_LobbyMsg::PKT_C2S_LobbyMsg(Network::PacketBuffer& buf) :
	m_msg(buf.readU8String())
{}

void PKT_C2S_LobbyMsg::serialize(Network::PacketBuffer& buf) const
{
	buf.writeU8String(m_msg);
}
