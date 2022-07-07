#include "pkt_s2c_pongend.hpp"

#include "network/packetbuffer.hpp"

IMPL_PACKET(PKT_S2C_PongEnd)

PKT_S2C_PongEnd::PKT_S2C_PongEnd(bool wasLastRound, RoundStats* stats, RoundStats* finalStats) :
	m_wasLastRound(wasLastRound), m_stats(stats), m_finalStats(finalStats)
{}

PKT_S2C_PongEnd::~PKT_S2C_PongEnd()
{
	delete m_stats;
	delete m_finalStats;
}

PKT_S2C_PongEnd::PKT_S2C_PongEnd(Network::PacketBuffer& buf)
{
	m_wasLastRound = buf.readBool();
	m_stats = new RoundStats();
	*m_stats = buf.read<RoundStats>();
	if (m_wasLastRound)
	{
		m_finalStats = new RoundStats();
		*m_finalStats = buf.read<RoundStats>();
	}
	else
	{
		m_finalStats = nullptr;
	}
}

void PKT_S2C_PongEnd::serialize(Network::PacketBuffer& buf) const
{
	buf.writeBool(m_wasLastRound);
	buf.write(*m_stats);
	if (m_wasLastRound)
		buf.write(*m_finalStats);
}
