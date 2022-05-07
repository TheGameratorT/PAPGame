#include "connection.hpp"
#include "private/connection.hpp"

#include <asio/ip/tcp.hpp>
#include <asio/connect.hpp>
#include <asio/read.hpp>
#include <asio/write.hpp>

#include "util/log.hpp"

using asio::ip::tcp;

NETWORK_BEGIN

Connection::Connection() :
	p(new Connection::private_t()),
	m_connected(false),
	m_pktListener(nullptr),
	m_connectHandler(nullptr),
	m_disconnectHandler(nullptr),
	m_errorHandler(nullptr)
{}

Connection::Connection(const ConnectionContext& context) :
	Connection()
{
	p->ctx = context;
}

Connection::~Connection()
{
	// DERIVED MUST CALL closeIfConnected()
	delete p;
}

void Connection::closeIfConnected()
{
	if (m_connected)
	{
		Log::warn("NetworkConnection", "Connection destroyed with an open connection!");
		close();
	}
}

ConnectionContext& Connection::getContext()
{
	return p->ctx;
}

void Connection::terminate()
{
	if (m_connected)
		p->ctx.socket->shutdown(asio::ip::tcp::socket::shutdown_both);
	p->ctx.socket->close();
}

void Connection::close()
{
	if (!m_connected)
		return;
	m_connected = false;

	asio::post(*p->ctx.io_context, [this](){ terminate(); });
}

void Connection::setIncomingPackets(const PacketListSpan& packetList)
{
	m_inPktList = packetList;
}

void Connection::setOutgoingPackets(const PacketListSpan& packetList)
{
	m_outPktList = packetList;
}

std::string Connection::getAddress()
{
	if (!m_connected)
		return "";
	return p->ctx.socket->remote_endpoint().address().to_string();
}

void Connection::clearOutgoingPacketQueue()
{
	m_writePkts.clear();
}

void Connection::sendPacket(const Packet& packet)
{
	/*if (!m_connected)
		throw std::runtime_error("Tried to send packet on closed connection");*/

	const PacketProfile& profile = packet.getProfile();
	u32 pktID = m_outPktList.find(profile);
	if (pktID == m_outPktList.size())
	{
		handleError(
			ConnectionError::SendPacketUnlisted,
			ConnectionError::Category::SendPacket,
			&profile
		);
		return;
	}
	auto* encPacket = new EncodedPacket(EncodedPacket::encode(packet, pktID));
	bool writeInProgress = !m_writePkts.empty();
	m_writePkts.emplace_back(encPacket);
	if (!writeInProgress && m_connected)
		writeOutgoingPacket();
}

void Connection::readIncomingPacketHeader()
{
	asio::async_read(
		*p->ctx.socket,
		asio::buffer(
			m_readPkt.data(),
			EncodedPacket::headerSize
		),
		[this](std::error_code error, std::size_t /*size*/)
		{
			if (!error)
			{
				readIncomingPacketBody();
			}
			else
			{
				if (checkDisconnected(error))
				{
					handleDisconnect();
				}
				else
				{
					handleError(
						ConnectionError::ReadPacketHeader,
						ConnectionError::Category::ReadPacket,
						&error
					);
				}
			}
		}
	);
}

void Connection::readIncomingPacketBody()
{
	u32 pktID = m_readPkt.id();
	if (pktID >= m_inPktList.size())
	{
		handleError(
			ConnectionError::ReadPacketInvalid,
			ConnectionError::Category::ReadPacket,
			&pktID
		);
		return;
	}

	u32 bodySize = m_readPkt.bodySize();
	std::size_t pktSize = bodySize + EncodedPacket::headerSize;
	if (pktSize > m_readPkt.size())
	{
		PacketReceiveBufExceedInfo info = { m_readPkt.id(), pktSize };
		handleError(
			ConnectionError::ReadPacketBufferExceeded,
			ConnectionError::Category::ReadPacket,
			&info
		);
		return;
	}

	asio::async_read(
		*p->ctx.socket,
		asio::buffer(
			m_readPkt.body(),
			bodySize
		),
		[this](std::error_code error, std::size_t /*size*/)
		{
			if (!error)
			{
				applyReceivedPacket();
				readIncomingPacketHeader();
			}
			else
			{
				if (checkDisconnected(error))
				{
					handleDisconnect();
				}
				else
				{
					handleError(
						ConnectionError::ReadPacketBody,
						ConnectionError::Category::ReadPacket,
						&error
					);
				}
			}
		}
	);
}

void Connection::writeOutgoingPacket()
{
	EncodedPacket& writePkt = *m_writePkts.front();

	asio::async_write(
		*p->ctx.socket,
		asio::buffer(
			writePkt.data(),
			writePkt.size()
		),
		[this](std::error_code error, std::size_t /*size*/)
		{
			if (!error)
			{
				m_writePkts.pop_front();
				if (!m_writePkts.empty())
					writeOutgoingPacket();
			}
			else
			{
				if (checkDisconnected(error))
				{
					handleDisconnect();
				}
				else
				{
					handleError(
						ConnectionError::SendPacketWrite,
						ConnectionError::Category::SendPacket,
						&error
					);
				}
			}
		}
	);
}

void Connection::applyReceivedPacket()
{
	u32 pktID = m_readPkt.id();
	const PacketProfile& profile = m_inPktList[pktID];
	char* pktBody = m_readPkt.body();
	PacketBuffer buf(std::vector<char>(pktBody, pktBody + m_readPkt.bodySize()));
	Packet* packet = profile.ctor(buf);
	if (m_pktListener)
		m_pktListener(pktID, *packet);
	delete packet;
}

bool Connection::checkDisconnected(const std::error_code& error)
{
	return (asio::error::eof == error) ||
		   (asio::error::connection_reset == error) ||
		   (asio::error::shut_down);
}

void Connection::handleDisconnect()
{
	close();
	if (m_disconnectHandler)
		m_disconnectHandler();
}

void Connection::handleErrorA(const ConnectionError& error)
{
	if (m_errorHandler)
		m_errorHandler(error);
}

template<typename ...Args> requires Constructible<ConnectionError, Args...>
void Connection::handleError(Args&&... args)
{ handleErrorA(ConnectionError(std::forward<Args>(args)...)); }

NETWORK_END
