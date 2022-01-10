#pragma once

#include <array>
#include <deque>
#include <utility>
#include <boost/asio.hpp>

#include "Packet.hpp"
#include "EncodedPacket.hpp"

class PacketSendError
{
public:
	enum Value
	{
		UNLISTED_PKT,
		WRITE_PKT
	};

	explicit PacketSendError(Value value, const void* arg) :
		m_value(value), m_arg(arg)
	{}

	[[nodiscard]] constexpr Value value() const
	{ return m_value; }

	template<typename T>
	constexpr const T* arg() const
	{ return static_cast<const T*>(m_arg); }

private:
	Value m_value;
	const void* m_arg;
};

class PacketSender
{
public:
	void setOutgoingPackets(const PacketProfile* const* profiles);
	void sendPacket(const Packet& packet);
	void setPacketSendErrorHandler(std::function<void(const PacketSendError&)> packetSendErrorHandler);

	template<size_t size>
	inline void setOutgoingPackets(const std::array<const PacketProfile*, size>& profiles) {
		setOutgoingPackets(profiles.data());
	}

protected:
	explicit PacketSender(boost::asio::ip::tcp::socket& socket, std::function<void()> connectionCloseFunction);

	void writeOutgoingPacket();

	boost::asio::ip::tcp::socket& m_socket;
	EncodedPacketQueue m_writePkts;
	const PacketProfile* const* m_outgoingPacketProfiles;

private:
	std::function<void()> m_connectionCloseFunction;
	std::function<void(const PacketSendError&)> m_packetSendErrorHandler;
};
