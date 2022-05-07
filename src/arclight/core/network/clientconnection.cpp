#include "clientconnection.hpp"

#include <asio/ip/tcp.hpp>
#include <asio/connect.hpp>

#include "util/log.hpp"
#include "private/connection.hpp"

using asio::ip::tcp;

NETWORK_BEGIN

ClientConnection::ClientConnection() :
	Connection(),
	m_connecting(false)
{}

ClientConnection::~ClientConnection()
{
	cancelConnect();
	closeIfConnected();
	if (m_io_thread.joinable())
		m_io_thread.join();
}

void ClientConnection::connect(const std::string& host, u16 port)
{
	if (m_connected)
	{
		Log::error("NetworkConnection", "Tried to reconnect on an active connection.");
		return;
	}

	auto* io_context = new asio::io_context();

	tcp::resolver resolver(*io_context);
	auto endpoints = resolver.resolve(host, std::to_string(port));

	auto* socket = new tcp::socket(*io_context);

	p->ctx = { io_context, socket };

	m_connecting = true;
	asio::async_connect(
		*socket,
		endpoints,
		[this](std::error_code error, const tcp::endpoint& /*endpoint*/)
		{
			m_connecting = false;
			if (!error)
			{
				m_connected = true;
				m_readPkt = EncodedPacket::createReader(512); // TODO: PLACE THIS SOMEWHERE CONVENIENT

				if (m_connectHandler)
					m_connectHandler();

				readIncomingPacketHeader();

				if (!m_writePkts.empty())
					writeOutgoingPacket();
			}
			else
			{
				handleError(
					ConnectionError::ConnectionFailed,
					ConnectionError::Category::Connect,
					&error
				);
				p->ctx.io_context->stop();
			}
		}
	);

	if (m_io_thread.joinable())
		m_io_thread.join();

	m_io_thread = std::thread([this](){
		p->ctx.io_context->run();
		delete p->ctx.socket;
		delete p->ctx.io_context;
	});
}

void ClientConnection::cancelConnect()
{
	if (!m_connecting)
		return;

	terminate();
}

void ClientConnection::terminate()
{
	Connection::terminate();
	p->ctx.io_context->stop();
}

bool ClientConnection::checkConnectCanceled(const std::error_code& error)
{
	return error == asio::error::operation_aborted;
}

NETWORK_END
