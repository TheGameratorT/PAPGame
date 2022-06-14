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
		const std::vector<std::unique_ptr<PlayerInfo>>& players,
		const std::vector<U8String>& chatHistory
	);

	~PKT_S2C_LobbyData() override;

	explicit PKT_S2C_LobbyData(Network::PacketBuffer& buf);
	void serialize(Network::PacketBuffer& buf) const override;

	[[nodiscard]] constexpr const std::vector<std::unique_ptr<PlayerInfo>>& getPlayers() const { return m_players; }
	[[nodiscard]] constexpr const std::vector<U8String>& getChatHistory() const { return m_chatHistory; }

private:
	std::vector<std::unique_ptr<PlayerInfo>> m_players;
	std::vector<U8String> m_chatHistory;
};
