#pragma once

#include "network/connectedclient.hpp"
#include "locale/unicodestring.hpp"

class PlayerInfo;
class PKT_C2S_LobbyMsg;
class PKT_C2S_PlayerReady;

class Player
{
public:
	Player(Network::ConnectedClientPtr client, U8String name);
	~Player();

	[[nodiscard]] Network::ConnectedClientPtr getClient() { return m_client; }
	[[nodiscard]] constexpr u32 getID() const { return m_id; }
	[[nodiscard]] constexpr const U8String& getName() const { return m_name; }
	[[nodiscard]] constexpr bool getReady() const { return m_isReady; }

	void onConnect();
	void onDisconnect();
	void onPacketReceived(Network::PacketID id, const Network::Packet& packet);

private:
	Network::ConnectedClientPtr m_client;
	u32 m_id;
	U8String m_name;
	bool m_isReady;

	void onLobbyMessage(const PKT_C2S_LobbyMsg& packet);
	void onReady(const PKT_C2S_PlayerReady& packet);

	PlayerInfo createInfo() const;
	void sendLobbyData();
};
