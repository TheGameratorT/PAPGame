#include "pkt_s2c_lobbydata.hpp"

IMPL_PACKET(PKT_S2C_LobbyData)

PKT_S2C_LobbyData::PKT_S2C_LobbyData(
	u32 playerID,
	const std::vector<std::unique_ptr<PlayerInfo>>& players,
	const std::vector<U8String>& chatHistory
)
{
	m_playerID = playerID;

	m_players.resize(players.size());
	for (SizeT i = 0; i < players.size(); i++)
		m_players[i] = std::make_unique<PlayerInfo>(*players[i]);

	m_chatHistory = chatHistory;
}

PKT_S2C_LobbyData::~PKT_S2C_LobbyData() = default;

PKT_S2C_LobbyData::PKT_S2C_LobbyData(Network::PacketBuffer& buf)
{
	m_playerID = buf.read<u32>();

	auto playerCount = buf.read<SizeT>();
	m_players.resize(playerCount);
	for (SizeT i = 0; i < playerCount; i++)
	{
		auto p = std::make_unique<PlayerInfo>();
		*p = buf.read<PlayerInfo>();
		m_players[i] = std::move(p);
	}

	auto chatCount = buf.read<SizeT>();
	m_chatHistory.resize(chatCount);
	for (SizeT i = 0; i < chatCount; i++)
		m_chatHistory[i] = buf.readU8String();
}

void PKT_S2C_LobbyData::serialize(Network::PacketBuffer& buf) const
{
	buf.write<u32>(m_playerID);

	buf.write<SizeT>(m_players.size());
	for (auto& p : m_players)
		buf.write(*p);

	buf.write<SizeT>(m_chatHistory.size());
	for (auto& c : m_chatHistory)
	{
		buf.writeU8String(c);
	}
}
