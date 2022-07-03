#include "pkt_s2c_typestate.hpp"

#include "network/packetbuffer.hpp"

IMPL_PACKET(PKT_S2C_TypeState)

PKT_S2C_TypeState::PKT_S2C_TypeState(bool isReady, u32 storyID, TypeStateStats* stats) :
	m_isReady(isReady), m_storyID(storyID), m_stats(stats)
{}

PKT_S2C_TypeState::~PKT_S2C_TypeState()
{
	delete m_stats;
}

PKT_S2C_TypeState::PKT_S2C_TypeState(Network::PacketBuffer& buf)
{
	m_isReady = buf.readBool();
	if (m_isReady)
	{
		m_storyID = buf.read<u32>();
		m_stats = nullptr;
	}
	else
	{
		m_stats = new TypeStateStats();
		*m_stats = buf.read<TypeStateStats>();
	}
}

void PKT_S2C_TypeState::serialize(Network::PacketBuffer& buf) const
{
	buf.writeBool(m_isReady);
	if (m_isReady)
	{
		buf.write(m_storyID);
	}
	else
	{
		buf.write(*m_stats);
	}
}
