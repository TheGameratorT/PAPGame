#include "connectionmanager.hpp"

#include "gameinfo.hpp"

#include "network/packettable.hpp"
#include "network/packet/pkt_c2s_handshake.hpp"
#include "network/packet/pkt_s2c_handshake.hpp"

#include "util/log.hpp"

using namespace Network;

#define PKT_LSTNR_JUMP(type, func) case PacketTable::idOfC2S<PKT_C2S_Handshake>(): func(static_cast<const type&>(packet)); break

class PacketListener
{
public:
	explicit PacketListener(ConnectionManager& connMgr, ConnectedClientHandle& client) :
		m_connMgr(connMgr), m_client(client)
	{}

	void operator()(PacketID id, const Packet& packet)
	{
		if (!m_client.didHandshake() && id != 0)
		{
			Log::error("Server", std::string("Client with ID ") +
								 std::to_string(m_client.getID()) + " tried to send packet before handshaking.");
			m_client.getConnection()->close();
			return;
		}

		switch(id)
		{
		PKT_LSTNR_JUMP(PKT_C2S_Handshake, onHandshake);
		}
	}

	void onHandshake(const PKT_C2S_Handshake& packet)
	{
		ConnectedClientPtr connection = m_client.getConnection();

		if (m_client.didHandshake())
		{
			Log::error("Server", std::string("Client with ID ") +
								 std::to_string(m_client.getID()) + " tried to handshake more than once.");
			connection->close();
			return;
		}

		if (packet.getMagic() != GameInfo::handshakeMagic)
		{
			connection->close();
			return;
		}

		if (packet.getVersion() != GameInfo::version)
		{
			Log::error("Server", std::string("Client with ID ") +
								 std::to_string(m_client.getID()) + " tried to connect with a different version.");
			connection->close();
			return;
		}

		// Now that an actual recognized connection was made we can set a proper error handler
		connection->setErrorHandler([&](const ConnectionError& error){
			std::ostringstream out;
			switch (error.value())
			{
			case ConnectionError::ConnectionFailed: {
				out << "Error connecting to server. (Code: " << error.getCode() << ")";
			} break;
			case ConnectionError::SendPacketUnlisted: {
				out << "Error trying to send packet, " <<
					error.arg<PacketProfile>()->name << " was not registered.";
			} break;
			case ConnectionError::SendPacketWrite: {
				out << "Error writing outgoing packet. (Code: " << error.getCode() << ")";
			} break;
			case ConnectionError::ReadPacketHeader: {
				out << "Error reading incoming packet header. (Code: " << error.getCode() << ")";
			} break;
			case ConnectionError::ReadPacketBody: {
				out << "Error reading incoming packet body. (Code: " << error.getCode() << ")";
			} break;
			case ConnectionError::ReadPacketInvalid: {
				out << "Error identifying incoming packet. (Packet ID: " << *error.arg<u32>() << ")";
			} break;
			case ConnectionError::ReadPacketBufferExceeded: {
				auto* info = error.arg<PacketReceiveBufExceedInfo>();
				out << "Error reading incoming packet body, "
					   "packet exceeds buffer size. (ID: "
					<< info->pktID << ", Size: " << info->pktSize << ")";
			} break;
			}
			// TODO: Send error to client
			Log::info("Server", out.str());
		});

		m_client.markHandshake();

		connection->sendPacket<PKT_S2C_Handshake>(GameInfo::handshakeMagic);

		// TODO: Name length security check

		if (m_connMgr.m_connectionListener)
			m_connMgr.m_connectionListener(m_client, packet.getPlayerName());
	}

private:
	ConnectionManager& m_connMgr;
	ConnectedClientHandle& m_client;
};

ConnectedClientHandle::~ConnectedClientHandle()
{
	delete m_listener;
}

ConnectionManager::ConnectionManager() :
	m_nextClientID(0)
{}

void ConnectionManager::startServer(u16 port)
{
	m_server.start(25565);

	m_server.setConnectionListener([&](ConnectedClientPtr other){
		auto* handle = new ConnectedClientHandle(
			m_nextClientID++,
			other,
			false
		);
		m_clients.emplace_back(handle);
		auto* listener = new PacketListener(*this, *handle);
		handle->setListener(listener);

		other->setIncomingPackets(PacketTable::packetsC2S);
		other->setOutgoingPackets(PacketTable::packetsS2C);

		other->setIncomingPacketListener(*listener);

		other->setErrorHandler([&](const ConnectionError& error){
			// Because no handshake has been done yet, we can't
			// verify if the connection is legitimate, in that case
			// just close the connection if an error occurs.
			other->close();
			return;
		});

		other->setDisconnectHandler([this, handle](){
			if (handle->didHandshake()) {
				if (m_disconnectionListener)
					m_disconnectionListener(*handle);
			}
			auto it = std::find_if(
				m_clients.begin(),
				m_clients.end(),
				[&](const std::unique_ptr<ConnectedClientHandle>& e){
					return e.get() == handle;
				}
			);
			m_clients.erase(it);
		});
	});
}

void ConnectionManager::stopServer()
{
	m_server.stop();
}
