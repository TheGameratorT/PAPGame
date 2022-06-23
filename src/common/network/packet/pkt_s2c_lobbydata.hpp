#pragma once

#include "network/packet.hpp"
#include "playerinfo.hpp"
#include "locale/unicodestring.hpp"

#include <string>
#include <vector>
#include <memory>

class PKT_S2C_LobbyData : public Network::Packet
{
public:
	DECL_PACKET()

	explicit PKT_S2C_LobbyData(
		u32 playerID,
		const std::vector<std::unique_ptr<PlayerInfo>>& players,
		const std::vector<U8String>& chatHistory
	);

	~PKT_S2C_LobbyData() override;

	explicit PKT_S2C_LobbyData(Network::PacketBuffer& buf);
	void serialize(Network::PacketBuffer& buf) const override;

	[[nodiscard]] constexpr u32 getPlayerID() const { return m_playerID; }
	[[nodiscard]] constexpr const std::vector<std::unique_ptr<PlayerInfo>>& getPlayers() const { return m_players; }
	[[nodiscard]] constexpr const std::vector<U8String>& getChatHistory() const { return m_chatHistory; }

private:
	u32 m_playerID;
	std::vector<std::unique_ptr<PlayerInfo>> m_players;
	std::vector<U8String> m_chatHistory;
};
