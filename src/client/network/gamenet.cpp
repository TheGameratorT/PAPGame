#include "gamenet.hpp"

#include "game.hpp"

#include "network/clientconnection.hpp"
#include "network/packettable.hpp"
#include "network/packet/pkt_c2s_handshake.hpp"
#include "network/packet/pkt_s2c_handshake.hpp"
#include "network/packet/pkt_s2c_lobbydata.hpp"
#include "network/packet/pkt_s2c_lobbymsg.hpp"
#include "network/packet/pkt_s2c_playerstate.hpp"
#include "network/packet/pkt_s2c_startgame.hpp"
#include "network/packet/pkt_s2c_typestate.hpp"
#include "network/packet/pkt_s2c_typeend.hpp"
#include "network/packet/pkt_s2c_gameend.hpp"
#include "network/packet/pkt_s2c_pongstart.hpp"
#include "network/packet/pkt_s2c_pongmove.hpp"
#include "network/packet/pkt_s2c_pongpoint.hpp"
#include "network/packet/pkt_s2c_pongend.hpp"
#include "scene/mgpongscene.hpp"
#include "scene/mgrythmscene.hpp"
#include "scene/mgtypewritescene.hpp"
#include "scene/resultsscene.hpp"
#include "gameinfo.hpp"
#include "util/log.hpp"

#include "player.hpp"
#include "roundstats.hpp"

using namespace Network;

namespace GameNet
{
static void onPacketReceived(Network::PacketID id, const Network::Packet& packet);
static void onLobbyData(const PKT_S2C_LobbyData& packet);
static void onLobbyMsg(const PKT_S2C_LobbyMsg& packet);
static void onPlayerState(const PKT_S2C_PlayerState& packet);
static void onStartGame(const PKT_S2C_StartGame& packet);
static void onTypeState(const PKT_S2C_TypeState& packet);
static void onTypeEnd(const PKT_S2C_TypeEnd& packet);
static void onGameEnd(const PKT_S2C_GameEnd& packet);
static void onPongStart(const PKT_S2C_PongStart& packet);
static void onPongMove(const PKT_S2C_PongMove& packet);
static void onPongPoint(const PKT_S2C_PongPoint& packet);
static void onPongEnd(const PKT_S2C_PongEnd& packet);
}

class PacketListener
{
public:
	void operator()(PacketID id, const Packet& packet)
	{
		if (!GameNet::getHandshakeDone() && id != PacketTable::idOfS2C<PKT_S2C_Handshake>())
		{
			Log::error("GameNetwork", std::string("Server tried to send packet before handshaking."));
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
			Game::onDisconnect();
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

	static void onPacketReceived(Network::PacketID id, const Network::Packet& packet)
	{
		switch (id)
		{
		PKT_S2C_JUMP(PKT_S2C_LobbyData, onLobbyData);
		PKT_S2C_JUMP(PKT_S2C_LobbyMsg, onLobbyMsg);
		PKT_S2C_JUMP(PKT_S2C_PlayerState, onPlayerState);
		PKT_S2C_JUMP(PKT_S2C_StartGame, onStartGame);
		PKT_S2C_JUMP(PKT_S2C_TypeState, onTypeState);
		PKT_S2C_JUMP(PKT_S2C_TypeEnd, onTypeEnd);
		PKT_S2C_JUMP(PKT_S2C_GameEnd, onGameEnd);
		PKT_S2C_JUMP(PKT_S2C_PongStart, onPongStart);
		PKT_S2C_JUMP(PKT_S2C_PongMove, onPongMove);
		PKT_S2C_JUMP(PKT_S2C_PongPoint, onPongPoint);
		PKT_S2C_JUMP(PKT_S2C_PongEnd, onPongEnd);
		}
	}

	static void onLobbyData(const PKT_S2C_LobbyData& packet)
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
			Log::error("GameNetwork", "The server did not send the local player information!");
			connection->close();
			return;
		}
		Game::setPlayer(localPlayer);

		auto& lobbyMsgs = Game::getLobbyMsgs();
		lobbyMsgs.clear();
		auto& newLobbyMsgs = packet.getChatHistory();
		lobbyMsgs.insert(lobbyMsgs.end(), newLobbyMsgs.begin(), newLobbyMsgs.end());
	}

	static void onLobbyMsg(const PKT_S2C_LobbyMsg& packet)
	{
		Game::getLobbyMsgs().push_back(packet.getMsg());
	}

	static void onPlayerState(const PKT_S2C_PlayerState& packet)
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
			Log::info("GameNetwork", "Tried to change state of non-existing player.");
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

	static void onStartGame(const PKT_S2C_StartGame& packet)
	{
		MiniGameType mgType = packet.getGameType();
		switch (mgType)
		{
		case MiniGameType::Pong:
			Game::switchScene<MGPongScene>();
			return;
		/*case MiniGameType::Rythm:
			Game::switchScene<MGRythmScene>();
			return;*/
		case MiniGameType::TypeWrite:
			Game::switchScene<MGTypeWriteScene>();
			return;
		default:
			break;
		}

		Log::error("GameNetwork", "Invalid minigame start requested!");
	}

	static void onTypeState(const PKT_S2C_TypeState& packet)
	{
		auto* scene = Game::getScene<MGTypeWriteScene>();
		if (!scene)
			return;

		bool isReady = packet.getIsReady();
		if (isReady)
		{
			scene->onTypingStart(packet.getStoryID());
			return;
		}

		scene->onTypingStats(*packet.getStats());
	}

	static void onTypeEnd(const PKT_S2C_TypeEnd& packet)
	{
		auto* scene = Game::getScene<MGTypeWriteScene>();
		if (!scene)
			return;

		scene->onRoundEnd(packet.getStats(), packet.getFinalStats());
	}

	static void onGameEnd(const PKT_S2C_GameEnd& packet)
	{
		for (auto& p : Game::getPlayers())
			p->setReady(false);
		Game::setResultStats(new RoundStats(*packet.getStats()));
		Game::switchScene<ResultsScene>();
	}

	static void onPongStart(const PKT_S2C_PongStart& packet)
	{
		auto* scene = Game::getScene<MGPongScene>();
		if (!scene)
			return;

		scene->onGameStart(packet.getPlayer1(), packet.getPlayer2());
	}

	static void onPongMove(const PKT_S2C_PongMove& packet)
	{
		auto* scene = Game::getScene<MGPongScene>();
		if (!scene)
			return;

		scene->onMoveObject(packet.getMovedObject(), packet.getPosition(), packet.getDirection());
	}

	static void onPongPoint(const PKT_S2C_PongPoint& packet)
	{
		auto* scene = Game::getScene<MGPongScene>();
		if (!scene)
			return;

		scene->onGamePoint(packet.getIsFinal(), packet.getPaddleID());
	}

	static void onPongEnd(const PKT_S2C_PongEnd& packet)
	{
		auto* scene = Game::getScene<MGPongScene>();
		if (!scene)
			return;

		scene->onRoundEnd(packet.getStats(), packet.getFinalStats());
	}
}
