#include "pkt_s2c_lobbydata.hpp"

IMPL_PACKET(PKT_S2C_LobbyData)

PKT_S2C_LobbyData::PKT_S2C_LobbyData(
	const std::vector<std::unique_ptr<PlayerInfo>>& players,
	const std::vector<U8String>& chatHistory
)
{
	m_players.resize(players.size());
	for (SizeT i = 0; i < players.size(); i++)
		m_players[i] = std::make_unique<PlayerInfo>(*players[i]);

	m_chatHistory = chatHistory;
}

PKT_S2C_LobbyData::~PKT_S2C_LobbyData() = default;

PKT_S2C_LobbyData::PKT_S2C_LobbyData(Network::PacketBuffer& buf)
{
	auto playerCount = buf.read<SizeT>();
	m_players.resize(playerCount);
	for (SizeT i = 0; i < playerCount; i++)
	{
		auto p = std::make_unique<PlayerInfo>();
		p->id = buf.read<u32>();
		p->name = buf.readU8String();
		p->team = buf.read<u32>();
		m_players[i] = std::move(p);
	}

	auto chatCount = buf.read<SizeT>();
	m_chatHistory.resize(chatCount);
	for (SizeT i = 0; i < chatCount; i++)
		m_chatHistory[i] = buf.readU8String();
}

void PKT_S2C_LobbyData::serialize(Network::PacketBuffer& buf) const
{
	buf.write<SizeT>(m_players.size());
	for (auto& p : m_players)
	{
		buf.write<u32>(p->id);
		buf.writeU8String(p->name);
		buf.write<u32>(p->team);
	}

	buf.write<SizeT>(m_chatHistory.size());
	for (auto& c : m_chatHistory)
	{
		buf.writeU8String(c);
	}
}
