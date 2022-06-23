#include "player.hpp"

Player::Player() : m_id(0), m_isReady(false) {}

Player::Player(const PlayerInfo& info) : Player()
{
	m_id = info.id;
	m_name = info.name;
	m_isReady = info.ready;
}

Player::~Player() = default;

void Player::onConnect()
{

}

void Player::onDisconnect()
{

}
