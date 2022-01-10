#pragma once

#include <thread>
#include <memory>
#include <cstdint>
#include <boost/asio.hpp>

#include "Packet.hpp"
#include "EncodedPacket.hpp"
#include "PacketListener.hpp"
#include "ConnectionListener.hpp"

class ConnectedClient;

class ServerConnection
{
public:
	ServerConnection(std::uint_fast16_t port);

	void start();
	void reset();
	void join();
	void terminate();

	void removeClient(ConnectedClient& connectedClient);

	void setConnectionListener(ConnectionListener& connectionListener);
	ConnectionListener& getConnectionListener();

	constexpr boost::asio::io_context& getIOContext()
	{ return m_io_context; }

	const std::vector<ConnectedClient*>& getConnectedClients();

private:
	void acceptIncomingConnections();

	boost::asio::io_context m_io_context;
	std::thread m_io_thread;
	boost::asio::ip::tcp::socket m_socket;
	boost::asio::ip::tcp::acceptor m_acceptor;
	std::vector<ConnectedClient*> m_connectedClients;
	ConnectionListener* m_connectionListener;
};
