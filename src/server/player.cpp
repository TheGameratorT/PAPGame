#include "player.hpp"

#include <utility>

#include "network/packettable.hpp"
#include "network/packet/pkt_c2s_handshake.hpp"

#define PKT_JUMP(type, func) case PacketTable::idOfC2S<type>(): func(static_cast<const type&>(packet)); break

Player::Player(Network::ConnectedClientPtr client, U8String name) :
	m_client(std::move(client)), m_name(std::move(name))
{}

Player::~Player() = default;

void Player::onConnect()
{

}

void Player::onDisconnect()
{

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

	/*switch (id)
	{
	PKT_JUMP(PKT_C2S_Handshake, );
	}*/
}
