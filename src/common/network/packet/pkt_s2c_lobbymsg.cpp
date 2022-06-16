#include "pkt_s2c_lobbymsg.hpp"

IMPL_PACKET(PKT_S2C_LobbyMsg)

PKT_S2C_LobbyMsg::PKT_S2C_LobbyMsg(U8String msg) :
	m_msg(std::move(msg))
{}

PKT_S2C_LobbyMsg::PKT_S2C_LobbyMsg(Network::PacketBuffer& buf) :
	m_msg(buf.readU8String())
{}

void PKT_S2C_LobbyMsg::serialize(Network::PacketBuffer& buf) const
{
	buf.writeU8String(m_msg);
}
