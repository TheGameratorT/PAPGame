#pragma once

#include <string>
#include <functional>

#include "types.hpp"
#include "network/clientconnection.hpp"
#include "locale/unicodestring.hpp"

class PKT_S2C_LobbyData;
class PKT_S2C_LobbyMsg;
class PKT_S2C_PlayerState;

namespace GameNet
{
	using ConnectCallback = std::function<void(bool)>;

	void init();
	void destroy();

	void connect(const std::string& address, u16 port, const ConnectCallback& callback);
	void disconnect();

	bool isConnected();
	const std::string& getServerAddress();

	void acceptConnection();

	Network::ClientConnection* getConnection();
	bool getHandshakeDone();

	void onPacketReceived(Network::PacketID id, const Network::Packet& packet);
	void onLobbyData(const PKT_S2C_LobbyData& packet);
	void onLobbyMsg(const PKT_S2C_LobbyMsg& packet);
	void onPlayerState(const PKT_S2C_PlayerState& packet);
}
