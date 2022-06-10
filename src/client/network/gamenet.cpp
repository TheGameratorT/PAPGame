#include "gamenet.hpp"

#include "network/clientconnection.hpp"
#include "network/packettable.hpp"
#include "network/packet/pkt_s2c_handshake.hpp"
#include "network/packet/pkt_c2s_handshake.hpp"
#include "gameinfo.hpp"
#include "util/log.hpp"

using namespace Network;

#define PKT_LSTNR_JUMP(id, type, func) case id: func(static_cast<const type&>(packet)); break

class PacketListener
{
public:
	void operator()(PacketID id, const Packet& packet)
	{
		if (!GameNet::getHandshakeDone() && id != 0)
		{
			Log::error("GameNet", std::string("Server tried to send packet before handshaking."));
			GameNet::disconnect();
			return;
		}

		switch(id)
		{
		PKT_LSTNR_JUMP(0, PKT_S2C_Handshake, onHandshake);
		}
	}

	void onHandshake(const PKT_S2C_Handshake& packet)
	{
		if (packet.getMagic() == GameInfo::handshakeMagic)
		{
			Log::error("GameNetwork", std::string("Handshake validated, proceeding..."));
			GameNet::acceptConnection();
		}
		else
		{
			Log::error("GameNetwork", std::string("Handshake magic mismatch! Aborting connection..."));
			GameNet::disconnect();
		}
	}
};

namespace GameNet
{
	ClientConnection* connection;
	PacketListener pktListener;
	ConnectCallback connectCallback;
	bool handshakeDone = false;

	void init()
	{
		connection = new ClientConnection();

		connection->setOutgoingPackets(PacketTable::packetsC2S);
		connection->setIncomingPackets(PacketTable::packetsS2C);
		connection->setIncomingPacketListener(pktListener);
		connection->setErrorHandler([&](const ConnectionError& error){
			std::ostringstream out;
			switch (error.value())
			{
			case ConnectionError::ConnectionFailed: {
				if (ClientConnection::checkConnectCanceled(error.getCode())) {
					out << "Error connecting to server. (Reason: Cancelled by user)";
				} else {
					out << "Error connecting to server. (Code: " << error.getCode() << ")";
				}
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
			Log::error("GameNetwork", out.str());
			if (!handshakeDone)
				connectCallback(false);
			connection->close();
		});
	}

	void destroy()
	{
		connection->close();
		delete connection;
	}

	void connect(const std::string& address, u16 port, const ConnectCallback& callback)
	{
		Log::info("GameNetwork", "Attempting to connect to: " + address + ":" + std::to_string(port));

		handshakeDone = false;
		connection->clearOutgoingPacketQueue();
		connection->connect(address, port);

		connectCallback = callback;

		connection->setConnectHandler([](){
			Log::info("GameNetwork", "Connected to server, performing handshake...");
		});
		connection->setDisconnectHandler([](){
			Log::info("GameNetwork", "Connection to server failed.");
			connectCallback(false);
		});

		// Queue a packet to send after connected
		connection->sendPacket<PKT_C2S_Handshake>(GameInfo::handshakeMagic, GameInfo::version);
	}

	void disconnect()
	{
		connection->close();
		// TODO: perhaps clear packet queue as well?
	}

	void acceptConnection()
	{
		handshakeDone = true;

		connection->setDisconnectHandler([](){
			Log::info("GameNetwork", "Disconnected from the server.");
		});

		connectCallback(true);
	}

	ClientConnection* getConnection()
	{
		return connection;
	}

	bool getHandshakeDone()
	{
		return handshakeDone;
	}
}
