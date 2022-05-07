#include "server.hpp"
#include "private/connectedclientcontext.hpp"

#include <memory>

#include <asio/io_context.hpp>
#include <asio/ip/tcp.hpp>

#include "util/log.hpp"

#include "connectedclient.hpp"

using asio::ip::tcp;

NETWORK_BEGIN

struct Server::private_t
{
	asio::io_context io_context;
	std::unique_ptr<tcp::socket> socket = nullptr;
	std::unique_ptr<tcp::acceptor> acceptor = nullptr;
};

Server::Server() :
	p(nullptr),
	m_active(false),
	m_connectionListener(nullptr)
{}

Server::~Server()
{
	if (m_active)
	{
		Log::warn("Server", "Server destroyed while running! Force stopping...");
		stop();
	}

	if (m_io_thread.joinable())
		m_io_thread.join();
}

void Server::start(u16 port)
{
	delete p;
	p = new private_t();

	p->socket = std::make_unique<tcp::socket>(p->io_context);
	p->acceptor = std::make_unique<tcp::acceptor>(p->io_context, tcp::endpoint(tcp::v4(), port));

	m_active = true;

	acceptIncomingConnections();

	if (m_io_thread.joinable())
		m_io_thread.join();

	m_io_thread = std::thread([this](){
		p->io_context.run();
		onStopped();
	});
}

void Server::stop()
{
	asio::post(p->io_context, [this](){
		for (const ConnectedClientPtr& connection : m_connectedClients)
			connection->detach();
		p->io_context.stop();
	});
	m_active = false;
}

void Server::waitUntilStopped()
{
	if (m_active)
		m_io_thread.join();
}

void Server::onStopped()
{
	m_connectedClients.clear(); // safe here because io_context is not in use anymore
	p->acceptor->close();
	p->socket->close();
	delete p;
}

void Server::acceptIncomingConnections()
{
	p->acceptor->async_accept(
		[this](std::error_code error, tcp::socket socket)
		{
			if (!error)
			{
				auto* socketPtr = new tcp::socket(std::move(socket));
				auto connection = ConnectedClient::create({ this, &p->io_context, socketPtr });
				connection->start();
				if (m_connectionListener)
					m_connectionListener(connection);
				m_connectedClients.push_back(connection);
			}
			acceptIncomingConnections();
		}
	);
}

void Server::unlinkConnection(ConnectedClient* connection)
{
	auto it = std::find_if(
		m_connectedClients.begin(),
		m_connectedClients.end(),
		[&](const ConnectedClientPtr& e){
			return e.get() == connection;
		}
	);
	if (it == m_connectedClients.end())
	{
		Log::warn("Server", "Tried to unlink unmanaged connection!");
		return;
	}
	m_connectedClients.erase(it);
}

NETWORK_END
