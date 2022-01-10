#include "PacketSender.hpp"

#include <iostream>

#define CONNECTION_CLOSED_ERROR 10054

using boost::asio::ip::tcp;

PacketSender::PacketSender(tcp::socket& socket, std::function<void()> connectionCloseFunction) :
	m_socket(socket),
	m_outgoingPacketProfiles(nullptr),
	m_connectionCloseFunction(std::move(connectionCloseFunction))
{}

void PacketSender::setOutgoingPackets(const PacketProfile* const* profiles)
{
	m_outgoingPacketProfiles = profiles;
}

void PacketSender::setPacketSendErrorHandler(std::function<void(const PacketSendError&)> packetSendErrorHandler)
{
	m_packetSendErrorHandler = std::move(packetSendErrorHandler);
}

void PacketSender::sendPacket(const Packet& packet)
{
	const PacketProfile& profile = packet.getProfile();
	std::uint_fast32_t pktID = PacketUtils::getPacketProfileID(&profile, m_outgoingPacketProfiles);
	if (pktID == INVALID_PACKET_ID)
	{
		m_packetSendErrorHandler(PacketSendError(PacketSendError::UNLISTED_PKT, &profile));
		return;
	}
	auto* encPacket = new EncodedPacket(
		EncodedPacket::encode(packet, pktID)
	);
	bool writeInProgress = !m_writePkts.empty();
	m_writePkts.emplace_back(encPacket);
	if (!writeInProgress)
	{
		writeOutgoingPacket();
	}
}

void PacketSender::writeOutgoingPacket()
{
	EncodedPacket& writePkt = *m_writePkts.front().get();
	boost::asio::async_write(
		m_socket,
		boost::asio::buffer(
			writePkt.data(),
			writePkt.size()
		),
		[this](boost::system::error_code error, std::size_t /*size*/)
		{
			if (!error)
			{
				m_writePkts.pop_front();
				if (!m_writePkts.empty())
					writeOutgoingPacket();
			}
			else
			{
				if (error.value() == CONNECTION_CLOSED_ERROR)
				{
					m_connectionCloseFunction();
				}
				else
				{
					m_packetSendErrorHandler(PacketSendError(PacketSendError::WRITE_PKT, &error));
				}
			}
		}
	);
}
