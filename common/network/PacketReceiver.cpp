#include "PacketReceiver.hpp"

#include <iostream>
#include <utility>

#define CONNECTION_CLOSED_ERROR 10054

using boost::asio::ip::tcp;

PacketReceiver::PacketReceiver(tcp::socket& socket, std::function<void()> connectionCloseFunction) :
	m_socket(socket),
	m_incomingPacketProfiles(nullptr),
	m_incomingPacketProfileCount(0),
	m_packetListener(nullptr),
	m_connectionCloseFunction(std::move(connectionCloseFunction))
{}

void PacketReceiver::allocIncomingPacketBuf(std::size_t bufferSize)
{
	m_readPkt = EncodedPacket::createReader(bufferSize);
}

void PacketReceiver::setIncomingPackets(const PacketProfile* const* profiles)
{
	m_incomingPacketProfiles = profiles;
	m_incomingPacketProfileCount = PacketUtils::getPacketProfileCount(profiles);
}

void PacketReceiver::setPacketListener(PacketListener& listener)
{
	m_packetListener = &listener;
}

void PacketReceiver::setPacketReceiveErrorHandler(std::function<void(const PacketReceiveError&)> packetReceiveErrorHandler)
{
	m_packetReceiveErrorHandler = std::move(packetReceiveErrorHandler);
}

void PacketReceiver::readIncomingPacketHeader()
{
	boost::asio::async_read(
		m_socket,
		boost::asio::buffer(
			m_readPkt.data(),
			EncodedPacket::headerSize
		),
		[this](boost::system::error_code error, std::size_t /*size*/)
		{
			if (!error)
			{
				readIncomingPacketBody();
			}
			else
			{
				if (error.value() == CONNECTION_CLOSED_ERROR)
				{
					m_connectionCloseFunction();
				}
				else
				{
					m_packetReceiveErrorHandler(PacketReceiveError(PacketReceiveError::READ_HEADER, &error));
				}
			}
		}
	);
}

void PacketReceiver::readIncomingPacketBody()
{
	std::uint_fast32_t bodySize = m_readPkt.bodySize();
	std::size_t pktSize = bodySize + EncodedPacket::headerSize;
	if (pktSize > m_readPkt.size())
	{
		PacketReceiveBufExceedInfo info{m_readPkt.id(), pktSize};
		m_packetReceiveErrorHandler(PacketReceiveError(PacketReceiveError::BUF_EXCEED, &info));
		return;
	}
	boost::asio::async_read(
		m_socket,
		boost::asio::buffer(
			m_readPkt.body(),
			bodySize
		),
		[this](boost::system::error_code error, std::size_t /*size*/)
		{
			if (!error)
			{
				applyReceivedPacket();
				readIncomingPacketHeader();
			}
			else
			{
				if (error.value() == CONNECTION_CLOSED_ERROR)
				{
					m_connectionCloseFunction();
				}
				else
				{
					m_packetReceiveErrorHandler(PacketReceiveError(PacketReceiveError::READ_BODY, &error));
				}
			}
		}
	);
}

void PacketReceiver::applyReceivedPacket()
{
	std::uint_fast32_t pktID = m_readPkt.id();
	if (pktID > m_incomingPacketProfileCount)
	{
		m_packetReceiveErrorHandler(PacketReceiveError(PacketReceiveError::APPLY, &pktID));
		return;
	}
	const PacketProfile* profile = PacketUtils::getPacketProfileByID(m_incomingPacketProfiles, pktID);
	char* pktBody = m_readPkt.body();
	ByteBuf buf(std::vector<char>(pktBody, pktBody + m_readPkt.bodySize()));
	Packet* packet = profile->constructor(buf);
	packet->apply(*m_packetListener);
	delete packet;
}
