#pragma once

#include "network/connectedclient.hpp"
#include "locale/unicodestring.hpp"

class Player
{
public:
	Player(Network::ConnectedClientPtr client, U8String name);
	~Player();

	[[nodiscard]] Network::ConnectedClientPtr getClient() { return m_client; }

	void onConnect();
	void onDisconnect();
	void onPacketReceived(Network::PacketID id, const Network::Packet& packet);

private:
	Network::ConnectedClientPtr m_client;
	U8String m_name;
};
