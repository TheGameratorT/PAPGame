#include "ConnectedClient.hpp"

#include <iostream>

#include "ServerConnection.hpp"

using boost::asio::ip::tcp;

ConnectedClient::ConnectedClient(ServerConnection& connection, tcp::socket socket) :
	m_connection(connection),
	m_socket(std::move(socket)),
	PacketReceiver(m_socket, [this]{ disconnect(); }),
	PacketSender(m_socket, [this]{ disconnect(); })
{
	m_connection.getConnectionListener().onClientConnected(*this);
	readIncomingPacketHeader();
}

void ConnectedClient::disconnect()
{
	m_connection.getConnectionListener().onClientDisconnected(*this);
	boost::asio::post(
		m_connection.getIOContext(),
		[this](){ terminate(); }
	);
	m_connection.removeClient(*this);
}

void ConnectedClient::terminate()
{
	m_socket.close();
}

boost::asio::ip::address ConnectedClient::getAddress()
{
	return m_socket.remote_endpoint().address();
}
