#pragma once

#include <vector>
#include <memory>
#include <deque>
#include <boost/asio.hpp>

#include "Packet.hpp"
#include "EncodedPacket.hpp"
#include "PacketListener.hpp"
#include "PacketReceiver.hpp"
#include "PacketSender.hpp"

class ServerConnection;

class ConnectedClient :
	public PacketReceiver,
	public PacketSender
{
public:
	ConnectedClient(ServerConnection& connection, boost::asio::ip::tcp::socket socket);
	void disconnect();
	void terminate();
	[[nodiscard]] boost::asio::ip::address getAddress();

private:
	ServerConnection& m_connection;
	boost::asio::ip::tcp::socket m_socket;
};
