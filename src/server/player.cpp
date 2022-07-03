#include "player.hpp"

#include <utility>

#include "server.hpp"

#include "network/packettable.hpp"
#include "network/packet/pkt_c2s_handshake.hpp"
#include "network/packet/pkt_c2s_lobbymsg.hpp"
#include "network/packet/pkt_c2s_playerready.hpp"
#include "network/packet/pkt_c2s_typestate.hpp"
#include "network/packet/pkt_s2c_lobbydata.hpp"
#include "network/packet/pkt_s2c_playerstate.hpp"
#include "math/math.hpp"
#include "util/log.hpp"
#include "util.hpp"

#include "minigame/mgtypewriter.hpp"

constexpr SizeT MAX_MSG_HISTORY_COUNT = 12;

Player::Player(Network::ConnectedClientPtr client, U8String name) :
	m_id(0),
	m_client(std::move(client)),
	m_name(std::move(name)),
	m_isReady(false),
	m_gameReady(false),
	m_charsTyped(0)
{}

Player::~Player() = default;

void Player::onConnect()
{
	m_id = Server::getNextPlayerID();
	sendLobbyData();

	for (auto& p : Server::getPlayers())
	{
		if (p.get() != this)
		{
			auto* pInfo = new PlayerInfo(); // sendPacket destroys it
			*pInfo = createInfo();
			p->getClient()->sendPacket<PKT_S2C_PlayerState>(m_id, PKT_S2C_PlayerState::Join, pInfo);
		}
	}

	U8String welcomeStr = m_name;
	welcomeStr.append(u8" has joined!");
	Server::sendChatMessage(welcomeStr);
}

void Player::onDisconnect()
{
	for (auto& p : Server::getPlayers())
	{
		if (p.get() != this)
			p->getClient()->sendPacket<PKT_S2C_PlayerState>(m_id, PKT_S2C_PlayerState::Leave, nullptr);
	}
}

void Player::onPacketReceived(Network::PacketID id, const Network::Packet& packet)
{
	if (id == PacketTable::idOfC2S<PKT_C2S_Handshake>())
	{
		// Client tried to handshake more than once
		// TODO: Send message to client saying he tried to handshake more than once
		m_client->close();
		return;
	}

	switch (id)
	{
	PKT_C2S_JUMP(PKT_C2S_LobbyMsg, onLobbyMessage);
	PKT_C2S_JUMP(PKT_C2S_PlayerReady, onReady);
	PKT_C2S_JUMP(PKT_C2S_TypeState, onTypeState);
	}
}

void Player::onLobbyMessage(const PKT_C2S_LobbyMsg& packet)
{
	U8String resultMsg = m_name;
	resultMsg.append(u8": ");
	resultMsg.append(packet.getMsg());
	Server::sendChatMessage(resultMsg);
}

void Player::onReady(const PKT_C2S_PlayerReady& packet)
{
	using PReadyType = PKT_C2S_PlayerReady::ReadyType;

	PReadyType ready = packet.getReady();

	if (Server::getGameStarted())
	{
		// Verifies that the client has entered the game and is waiting for others
		if (ready == PReadyType::GameReady)
		{
			m_gameReady = true;

			bool allReady = true;
			for (auto& p : Server::getPlayers())
			{
				if (!p->getGameReady())
					allReady = false;
			}
			if (allReady)
				Server::notifyClientsReady();
		}
	}
	else
	{
		// Verifies that the client has clicked the ready button
		m_isReady = ready == PReadyType::Ready;

		bool allReady = true;
		for (auto& p : Server::getPlayers())
		{
			if (!p->getReady())
				allReady = false;
			p->getClient()->sendPacket<PKT_S2C_PlayerState>(m_id, PKT_S2C_PlayerState::Ready, m_isReady);
		}

		if (allReady)
			Server::startRandomMinigame();
	}
}

void Player::onTypeState(const PKT_C2S_TypeState& packet)
{
	u32 charsTyped = packet.getCharsTyped();
	if (charsTyped < m_charsTyped)
	{
		Log::warn("Player", std::string("Player \"") +
			Util::utf8AsString(m_name) + "\" with ID " + std::to_string(m_id) + " sent invalid typing information");
		return;
	}
	m_charsTyped = charsTyped;
}

PlayerInfo Player::createInfo() const
{
	PlayerInfo playerInfo;
	playerInfo.id = this->getID();
	playerInfo.name = this->getName();
	playerInfo.team = 0;
	playerInfo.ready = this->getReady();
	return playerInfo;
}

void Player::sendLobbyData()
{
	const auto& players = Server::getPlayers();
	const auto& lobbyMsgs = Server::getLobbyChat();

	SizeT playerCount = players.size();
	SizeT msgCount = lobbyMsgs.size();
	SizeT sendMsgCount = Math::min(msgCount, MAX_MSG_HISTORY_COUNT);

	std::vector<std::unique_ptr<PlayerInfo>> playerInfos(playerCount);
	for (SizeT i = 0; i < playerCount; i++)
	{
		auto playerInfo = std::make_unique<PlayerInfo>();
		*playerInfo = players[i]->createInfo();
		playerInfos[i] = std::move(playerInfo);
	}

	std::vector<U8String> sendLobbyMsgs(sendMsgCount);
	for (SizeT i = msgCount - sendMsgCount, j = 0; i < msgCount; i++, j++)
		sendLobbyMsgs[j] = lobbyMsgs[i];

	m_client->sendPacket<PKT_S2C_LobbyData>(m_id, playerInfos, sendLobbyMsgs);
}
