#pragma once

#include <array>
#include <utility>
#include <boost/asio.hpp>

#include "Packet.hpp"
#include "EncodedPacket.hpp"

class PacketReceiveError
{
public:
	enum Value
	{
		READ_HEADER,
		READ_BODY,
		BUF_EXCEED,
		APPLY
	};

	explicit PacketReceiveError(Value value, const void* arg) :
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

struct PacketReceiveBufExceedInfo
{
	std::uint32_t pktID;
	std::size_t pktSize;
};

class PacketReceiver
{
public:
	void allocIncomingPacketBuf(std::size_t bufferSize);
	void setIncomingPackets(const PacketProfile* const* profiles);
	void setPacketListener(PacketListener& listener);
	void setPacketReceiveErrorHandler(std::function<void(const PacketReceiveError&)> packetReceiveErrorHandler);

	template<size_t size>
	inline void setIncomingPackets(const std::array<const PacketProfile*, size>& profiles) {
		setIncomingPackets(profiles.data());
	}

protected:
	explicit PacketReceiver(boost::asio::ip::tcp::socket& socket, std::function<void()> connectionCloseFunction);

	void readIncomingPacketHeader();
	void readIncomingPacketBody();
	void applyReceivedPacket();

	boost::asio::ip::tcp::socket& m_socket;
	EncodedPacket m_readPkt;
	const PacketProfile* const* m_incomingPacketProfiles;
	std::uint_fast32_t m_incomingPacketProfileCount;
	PacketListener* m_packetListener;

private:
	std::function<void()> m_connectionCloseFunction;
	std::function<void(const PacketReceiveError&)> m_packetReceiveErrorHandler;
};
