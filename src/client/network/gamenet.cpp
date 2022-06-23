#include "gamenet.hpp"

#include "game.hpp"

#include "network/clientconnection.hpp"
#include "network/packettable.hpp"
#include "network/packet/pkt_c2s_handshake.hpp"
#include "network/packet/pkt_s2c_handshake.hpp"
#include "network/packet/pkt_s2c_lobbydata.hpp"
#include "network/packet/pkt_s2c_lobbymsg.hpp"
#include "network/packet/pkt_s2c_playerstate.hpp"
#include "gameinfo.hpp"
#include "util/log.hpp"

#include "player.hpp"

using namespace Network;

class PacketListener
{
public:
	void operator()(PacketID id, const Packet& packet)
	{
		if (!GameNet::getHandshakeDone() && id != PacketTable::idOfS2C<PKT_S2C_Handshake>())
		{
			Log::error("GameNet", std::string("Server tried to send packet before handshaking."));
			GameNet::disconnect();
			return;
		}

		if (id == PacketTable::idOfS2C<PKT_S2C_Handshake>())
		{
			PKT_CALL(PKT_S2C_Handshake, onHandshake);
			return;
		}

		GameNet::onPacketReceived(id, packet);
	}

	void onHandshake(const PKT_S2C_Handshake& packet)
	{
		if (packet.getMagic() == GameInfo::handshakeMagic)
		{
			Log::error("GameNetwork", std::string("Handshake validated, proceeding..."));
			GameNet::acceptConnection();
		}
		else
		{
			Log::error("GameNetwork", std::string("Handshake magic mismatch! Aborting connection..."));
			GameNet::disconnect();
		}
	}
};

namespace GameNet
{
	ClientConnection* connection;
	PacketListener pktListener;
	ConnectCallback connectCallback;
	bool handshakeDone = false;
	bool varIsConnected = false;
	std::string serverAddress;

	void init()
	{
		connection = new ClientConnection();

		connection->setOutgoingPackets(PacketTable::packetsC2S);
		connection->setIncomingPackets(PacketTable::packetsS2C);
		connection->setIncomingPacketListener(pktListener);
		connection->setErrorHandler([&](const ConnectionError& error){
			std::ostringstream out;
			switch (error.value())
			{
			case ConnectionError::ConnectionFailed: {
				if (ClientConnection::checkConnectCanceled(error.getCode())) {
					out << "Error connecting to server. (Reason: Cancelled by user)";
				} else {
					out << "Error connecting to server. (Code: " << error.getCode() << ")";
				}
			} break;
			case ConnectionError::SendPacketUnlisted: {
				out << "Error trying to send packet, " <<
					error.arg<PacketProfile>()->name << " was not registered.";
			} break;
			case ConnectionError::SendPacketWrite: {
				out << "Error writing outgoing packet. (Code: " << error.getCode() << ")";
			} break;
			case ConnectionError::ReadPacketHeader: {
				out << "Error reading incoming packet header. (Code: " << error.getCode() << ")";
			} break;
			case ConnectionError::ReadPacketBody: {
				out << "Error reading incoming packet body. (Code: " << error.getCode() << ")";
			} break;
			case ConnectionError::ReadPacketInvalid: {
				out << "Error identifying incoming packet. (Packet ID: " << *error.arg<u32>() << ")";
			} break;
			case ConnectionError::ReadPacketBufferExceeded: {
				auto* info = error.arg<PacketReceiveBufExceedInfo>();
				out << "Error reading incoming packet body, "
					   "packet exceeds buffer size. (ID: "
					<< info->pktID << ", Size: " << info->pktSize << ")";
			} break;
			}
			Log::error("GameNetwork", out.str());
			if (!handshakeDone)
				connectCallback(false);
			connection->close();
		});
	}

	void destroy()
	{
		connection->close();
		delete connection;
	}

	void connect(const std::string& address, u16 port, const ConnectCallback& callback)
	{
		Log::info("GameNetwork", "Attempting to connect to: " + address + ":" + std::to_string(port));

		serverAddress = address;

		handshakeDone = false;
		connection->clearOutgoingPacketQueue();
		connection->connect(address, port);

		connectCallback = callback;

		connection->setConnectHandler([](){
			Log::info("GameNetwork", "Connected to server, performing handshake...");
		});
		connection->setDisconnectHandler([](){
			Log::info("GameNetwork", "Connection to server failed.");
			connectCallback(false);
		});

		// Queue a packet to send after connected
		connection->sendPacket<PKT_C2S_Handshake>(GameInfo::handshakeMagic, GameInfo::version, Game::getPlayerName());
	}

	void disconnect()
	{
		connection->close();
		// TODO: perhaps clear packet queue as well?
	}

	bool isConnected()
	{
		return varIsConnected;
	}

	const std::string& getServerAddress()
	{
		return serverAddress;
	}

	void acceptConnection()
	{
		handshakeDone = true;
		varIsConnected = true;

		connection->setDisconnectHandler([](){
			varIsConnected = false;
			Log::info("GameNetwork", "Disconnected from the server.");
		});

		connectCallback(true);
	}

	ClientConnection* getConnection()
	{
		return connection;
	}

	bool getHandshakeDone()
	{
		return handshakeDone;
	}

	void onPacketReceived(Network::PacketID id, const Network::Packet& packet)
	{
		switch (id)
		{
		PKT_S2C_JUMP(PKT_S2C_LobbyData, onLobbyData);
		PKT_S2C_JUMP(PKT_S2C_LobbyMsg, onLobbyMsg);
		PKT_S2C_JUMP(PKT_S2C_PlayerState, onPlayerState);
		}
	}

	void onLobbyData(const PKT_S2C_LobbyData& packet)
	{
		u32 playerID = packet.getPlayerID();
		Game::setPlayerID(playerID);

		Game::getPlayers().clear();

		Player* localPlayer = nullptr;
		for (const auto &player : packet.getPlayers())
		{
			Player* newPlayer = Game::createPlayer(*player);
			if (newPlayer->getID() == playerID)
				localPlayer = newPlayer;
		}

		if (!localPlayer)
		{
			Log::error("GameNet", "The server did not send the local player information!");
			connection->close();
			return;
		}
		Game::setPlayer(localPlayer);

		auto& lobbyMsgs = Game::getLobbyMsgs();
		lobbyMsgs.clear();
		auto& newLobbyMsgs = packet.getChatHistory();
		lobbyMsgs.insert(lobbyMsgs.end(), newLobbyMsgs.begin(), newLobbyMsgs.end());
	}

	void onLobbyMsg(const PKT_S2C_LobbyMsg& packet)
	{
		Game::getLobbyMsgs().push_back(packet.getMsg());
	}

	void onPlayerState(const PKT_S2C_PlayerState& packet)
	{
		using PState = PKT_S2C_PlayerState;

		PState::State state = packet.getState();

		if (state == PState::Join)
		{
			const PlayerInfo& playerInfo = packet.getPlayerInfo();
			Game::createPlayer(playerInfo);
			return;
		}

		u32 playerID = packet.getPlayerID();
		Player* player = Game::getPlayerByID(playerID);
		if (!player)
		{
			Log::info("GameNet", "Tried to change state of non-existing player.");
			return;
		}

		if (state == PState::Leave)
		{
			Game::removePlayer(playerID);
			return;
		}

		if (state == PState::Ready)
		{
			player->setReady(packet.getReady());
			return;
		}
	}
}
