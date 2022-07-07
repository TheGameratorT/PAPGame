#pragma once

#include "network/connectedclient.hpp"
#include "locale/unicodestring.hpp"

class PlayerInfo;
class PKT_C2S_LobbyMsg;
class PKT_C2S_PlayerReady;
class PKT_C2S_TypeState;
class PKT_C2S_PongMove;

class Player
{
public:
	Player(Network::ConnectedClientPtr client, U8String name);
	~Player();

	[[nodiscard]] Network::ConnectedClientPtr getClient() { return m_client; }
	[[nodiscard]] constexpr u32 getID() const { return m_id; }
	[[nodiscard]] constexpr const U8String& getName() const { return m_name; }
	[[nodiscard]] constexpr bool getReady() const { return m_isReady; }

	constexpr void setGameReady(bool gameReady) { m_gameReady = gameReady; }
	[[nodiscard]] constexpr bool getGameReady() const { return m_gameReady; }

	constexpr void setCharsTyped(u32 charsTyped) { m_charsTyped = charsTyped; }
	[[nodiscard]] constexpr u32 getCharsTyped() const { return m_charsTyped; }

	constexpr void setPreviousPlace(u32 prevPlace) { m_prevPlace = prevPlace; }
	[[nodiscard]] constexpr u32 getPreviousPlace() const { return m_prevPlace; }

	constexpr void setPoints(u32 points) { m_points = points; }
	[[nodiscard]] constexpr u32 getPoints() const { return m_points; }

	constexpr void setGamePoints(u32 gamePoints) { m_gamePoints = gamePoints; }
	[[nodiscard]] constexpr u32 getGamePoints() const { return m_gamePoints; }

	void prepareForNextGame();

	void onConnect();
	void onDisconnect();
	void onPacketReceived(Network::PacketID id, const Network::Packet& packet);

private:
	Network::ConnectedClientPtr m_client;
	u32 m_id;
	U8String m_name;
	bool m_isReady;
	bool m_gameReady;
	u32 m_charsTyped;
	u32 m_prevPlace;
	u32 m_points;
	u32 m_gamePoints;

	void onLobbyMessage(const PKT_C2S_LobbyMsg& packet);
	void onReady(const PKT_C2S_PlayerReady& packet);
	void onTypeState(const PKT_C2S_TypeState& packet);
	void onPongMove(const PKT_C2S_PongMove& packet);

	[[nodiscard]] PlayerInfo createInfo() const;
	void sendLobbyData();
};
