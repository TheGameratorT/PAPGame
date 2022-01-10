#pragma once

#include <string>
#include <deque>
#include <cstdint>
#include <thread>
#include <memory>
#include <boost/asio.hpp>

#include "Packet.hpp"
#include "EncodedPacket.hpp"
#include "PacketListener.hpp"
#include "PacketReceiver.hpp"
#include "PacketSender.hpp"

class ClientConnection :
	public PacketReceiver,
	public PacketSender
{
public:
	ClientConnection();

	void connect(const std::string& host, std::uint_fast16_t port);
	void disconnect();
	void reset();
	void join();
	[[nodiscard]] bool isConnected() const;

private:
	void terminate();

	boost::asio::io_context m_io_context;
	std::thread m_io_thread;
	boost::asio::ip::tcp::socket m_socket;
	bool m_connected;
};
