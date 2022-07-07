#include "pkt_s2c_gameend.hpp"

#include "network/packetbuffer.hpp"

IMPL_PACKET(PKT_S2C_GameEnd)

PKT_S2C_GameEnd::PKT_S2C_GameEnd(RoundStats* stats) :
	m_stats(stats)
{}

PKT_S2C_GameEnd::~PKT_S2C_GameEnd()
{
	delete m_stats;
}

PKT_S2C_GameEnd::PKT_S2C_GameEnd(Network::PacketBuffer& buf)
{
	m_stats = new RoundStats(buf.read<RoundStats>());
}

void PKT_S2C_GameEnd::serialize(Network::PacketBuffer& buf) const
{
	buf.write(*m_stats);
}
