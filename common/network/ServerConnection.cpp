#include "ServerConnection.hpp"

#include <iostream>

#include "ConnectedClient.hpp"

using boost::asio::ip::tcp;

ServerConnection::ServerConnection(std::uint_fast16_t port) :
	m_socket(m_io_context),
	m_acceptor(m_io_context, tcp::endpoint(tcp::v4(), port))
{};

void ServerConnection::start()
{
	acceptIncomingConnections();
	m_io_thread = std::thread([this](){ m_io_context.run(); });
}

void ServerConnection::reset()
{
	m_io_context.restart();
}

void ServerConnection::join()
{
	m_io_thread.join();
}

void ServerConnection::terminate()
{
	m_socket.close();
	m_io_context.stop();
}

void ServerConnection::acceptIncomingConnections()
{
	m_acceptor.async_accept(
		[this](boost::system::error_code error, tcp::socket socket)
		{
			if (!error)
			{
				m_connectedClients.push_back(new ConnectedClient(*this, std::move(socket)));
			}
			acceptIncomingConnections();
		}
	);
}

void ServerConnection::removeClient(ConnectedClient& connectedClient)
{
	auto pos = std::find(m_connectedClients.begin(), m_connectedClients.end(), &connectedClient);
	if (pos != m_connectedClients.end())
	{
		m_connectedClients.erase(pos);
		delete &connectedClient;
	}
}

void ServerConnection::setConnectionListener(ConnectionListener& connectionListener)
{
	m_connectionListener = &connectionListener;
}

ConnectionListener& ServerConnection::getConnectionListener()
{
	return *m_connectionListener;
}

const std::vector<ConnectedClient*>& ServerConnection::getConnectedClients()
{
	return m_connectedClients;
}
