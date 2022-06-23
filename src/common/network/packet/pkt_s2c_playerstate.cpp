#include "pkt_s2c_playerstate.hpp"

IMPL_PACKET(PKT_S2C_PlayerState)

PKT_S2C_PlayerState::PKT_S2C_PlayerState(u32 playerID, State state, bool ready) :
	m_playerID(playerID), m_state(state), m_playerInfo(nullptr), m_ready(ready)
{}

PKT_S2C_PlayerState::PKT_S2C_PlayerState(u32 playerID, State state, PlayerInfo* info) :
	m_playerID(playerID), m_state(state), m_playerInfo(info), m_ready(false)
{}

PKT_S2C_PlayerState::~PKT_S2C_PlayerState()
{
	delete m_playerInfo;
}

PKT_S2C_PlayerState::PKT_S2C_PlayerState(Network::PacketBuffer& buf)
{
	m_playerID = buf.read<u32>();
	m_state = static_cast<State>(buf.read<u32>());

	if (m_state == State::Join)
	{
		m_playerInfo = new PlayerInfo();
		*m_playerInfo = buf.read<PlayerInfo>();
	}
	else
	{
		m_playerInfo = nullptr;
	}

	m_ready = m_state == State::Ready && buf.readBool();
}

void PKT_S2C_PlayerState::serialize(Network::PacketBuffer& buf) const
{
	buf.write<u32>(m_playerID);
	buf.write<u32>(m_state);
	if (m_state == State::Join)
		buf.write(*m_playerInfo);
	else if (m_state == State::Ready)
		buf.write(m_ready);
}
