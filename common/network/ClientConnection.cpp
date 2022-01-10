#include "ClientConnection.hpp"

#include <iostream>

using boost::asio::ip::tcp;

ClientConnection::ClientConnection() :
	m_socket(m_io_context),
	m_connected(false),
	PacketReceiver(m_socket, [this]{ terminate(); }),
	PacketSender(m_socket, [this]{ terminate(); })
{}

void ClientConnection::connect(const std::string& host, std::uint_fast16_t port)
{
	tcp::resolver resolver(m_io_context);
	auto endpoints = resolver.resolve(host, std::to_string(port));

	boost::asio::async_connect(
		m_socket,
		endpoints,
		[this](boost::system::error_code error, const tcp::endpoint& /*endpoint*/)
		{
			if (!error)
			{
				m_connected = true;
				readIncomingPacketHeader();
			}
			else
			{
				std::cout << "Error connecting to server. (Code: " << error << ")" << std::endl;
			}
		}
	);

	m_io_thread = std::thread([this](){ m_io_context.run(); });
}

void ClientConnection::disconnect()
{
	boost::asio::post(m_io_context, [this](){ terminate(); });
}

void ClientConnection::reset()
{
	m_io_context.restart();
	m_socket = tcp::socket(m_io_context);
	m_writePkts.clear();
}

void ClientConnection::join()
{
	m_io_thread.join();
}

void ClientConnection::terminate()
{
	m_socket.close();
	m_io_context.stop();
	m_connected = false;
}

bool ClientConnection::isConnected() const
{
	return m_connected;
}
