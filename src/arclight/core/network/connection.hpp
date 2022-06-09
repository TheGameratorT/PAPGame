#pragma once

#include <thread>
#include <functional>
#include <deque>
#include <memory>

#include "packet.hpp"
#include "packetlistspan.hpp"
#include "encodedpacket.hpp"
#include "connectionerror.hpp"

struct PacketReceiveBufExceedInfo
{
	std::uint32_t pktID;
	std::size_t pktSize;
};

NETWORK_BEGIN

class ConnectionContext;

class Connection
{
public:
	using PacketListener = std::function<void(PacketID, Packet&)>;
	using ConnectHandler = std::function<void()>;
	using DisconnectHandler = std::function<void()>;
	using ErrorHandler = std::function<void(const ConnectionError&)>;

	ConnectionContext& getContext();

	[[nodiscard]] constexpr bool isConnected() const
	{ return m_connected; }

	void close();

	void setIncomingPackets(const PacketListSpan& packetList);

	void setOutgoingPackets(const PacketListSpan& packetList);

	inline void setIncomingPacketListener(const PacketListener& listener)
	{ m_pktListener = listener; }

	inline void setConnectHandler(const ConnectHandler& connectHandler)
	{ m_connectHandler = connectHandler; }

	inline void setDisconnectHandler(const DisconnectHandler& disconnectHandler)
	{ m_disconnectHandler = disconnectHandler; }

	inline void setErrorHandler(const ErrorHandler& errorHandler)
	{ m_errorHandler = errorHandler; }

	std::string getAddress();

	void clearOutgoingPacketQueue();

	void sendPacket(const Packet& packet);

	template<PacketType T, typename ...Args> requires CC::Constructible<T, Args...>
	inline void sendPacket(Args&&... args)
	{ sendPacket(T(std::forward<Args>(args)...)); }

protected:
	Connection();
	explicit Connection(const ConnectionContext& context);
	virtual ~Connection();

	using EncodedPacketQueue = std::deque<std::unique_ptr<EncodedPacket>>;

	struct private_t;
	private_t* p;

	bool m_connected;

	PacketListSpan m_inPktList;
	PacketListSpan m_outPktList;

	PacketListener m_pktListener;
	ConnectHandler m_connectHandler;
	DisconnectHandler m_disconnectHandler;
	ErrorHandler m_errorHandler;

	EncodedPacket m_readPkt;
	EncodedPacketQueue m_writePkts;

	void closeIfConnected();
	virtual void terminate();

	void readIncomingPacketHeader();
	void readIncomingPacketBody();
	void writeOutgoingPacket();

	void applyReceivedPacket();

	static bool checkDisconnected(const std::error_code& error);

	void handleDisconnect();

	void handleErrorA(const ConnectionError& error);

	template<typename ...Args> requires CC::Constructible<ConnectionError, Args...>
	inline void handleError(Args&&... args);
};

NETWORK_END
