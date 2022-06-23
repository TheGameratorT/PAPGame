#include "pkt_s2c_startgame.hpp"

IMPL_PACKET(PKT_S2C_StartGame)

PKT_S2C_StartGame::PKT_S2C_StartGame(MiniGameType gameType) :
	m_gameType(gameType)
{}

PKT_S2C_StartGame::PKT_S2C_StartGame(Network::PacketBuffer& buf) :
	m_gameType(static_cast<MiniGameType>(buf.read<u32>()))
{}

void PKT_S2C_StartGame::serialize(Network::PacketBuffer& buf) const
{
	buf.write<u32>(m_gameType);
}
