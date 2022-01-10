#include <iostream>
#include <string>
#include <exception>

#include <chrono>
#include <thread>
#include <sstream>

#include "GameInfo.hpp"

#include "network/Packet.hpp"
#include "network/EncodedPacket.hpp"
#include "network/PacketList.hpp"
#include "network/ConnectedClient.hpp"
#include "network/ServerConnection.hpp"
#include "network/ServerPacketListener.hpp"

#include "network/packet/s2c/S2CDisconnectPacket.hpp"
#include "network/packet/s2c/S2CConnectReplyPacket.hpp"
#include "network/packet/s2c/S2CChatMessagePacket.hpp"

class ServerPlayPacketHandler;

class ConnectionHandler : public ConnectionListener
{
public:
	explicit ConnectionHandler(ServerConnection& server);

	void onClientConnected(ConnectedClient& conClient) override;
	void onClientDisconnected(ConnectedClient& conClient) override;
	void sendPacketExcluding(const Packet& packet, ConnectedClient& exclude);

private:
	ServerConnection& m_server;
	std::deque<ServerPlayPacketHandler*> m_pktHandlers;
};

class ServerPlayPacketHandler : public ServerPacketListener
{
public:
	explicit ServerPlayPacketHandler(ConnectionHandler& conHandler, ConnectedClient& connection) :
		m_conHandler(conHandler),
		m_connection(connection)
	{}

	void onDisconnect(const C2SDisconnectPacket& packet) override
	{
		m_connection.disconnect();
	}

	void onClientInfo(const C2SClientInfoPacket& packet) override
	{
		using namespace std::chrono_literals;

		std::uint_fast32_t protocol = packet.getProtocol();
		if (protocol != GameInfo::protocolVersion)
		{
			std::string errMsg = "Invalid protocol version, expected "
								 + std::to_string(GameInfo::protocolVersion) + " but got " + std::to_string(protocol);
			m_connection.sendPacket(S2CConnectReplyPacket(false, errMsg));
			return;
		}

		const std::string& name = packet.getName();
		m_name = name;
		std::cout << name << " connected." << std::endl;

		std::this_thread::sleep_for(2000ms);

		m_connection.sendPacket(S2CConnectReplyPacket(true));
		m_connection.sendPacket(S2CChatMessagePacket("Welcome to the server!"));
		m_conHandler.sendPacketExcluding(S2CChatMessagePacket(name + " has joined the server."), m_connection);
	}

	void onChatMessage(const C2SChatMessagePacket& packet) override
	{
		const std::string& message = packet.getMessage();
		std::string result = m_name + ": " + message;
		std::cout << result << std::endl;
		m_conHandler.sendPacketExcluding(S2CChatMessagePacket(result), m_connection);
	}

	ConnectedClient& getConnection()
	{
		return m_connection;
	}

	const std::string& getName()
	{
		return m_name;
	}

private:
	ConnectionHandler& m_conHandler;
	ConnectedClient& m_connection;
	std::string m_name;
};

ConnectionHandler::ConnectionHandler(ServerConnection& server) :
	m_server(server)
{}

void ConnectionHandler::onClientConnected(ConnectedClient& conClient)
{
	std::cout << "Accepted connection from " << conClient.getAddress().to_string() << std::endl;

	auto* pktHandler = new ServerPlayPacketHandler(*this, conClient);
	m_pktHandlers.push_back(pktHandler);

	conClient.allocIncomingPacketBuf(512);
	conClient.setIncomingPackets(PacketList::C2SPackets);
	conClient.setOutgoingPackets(PacketList::S2CPackets);
	conClient.setPacketListener(*pktHandler);
	conClient.setPacketReceiveErrorHandler(
		[&conClient](const PacketReceiveError& error){
			std::ostringstream out;
			out << "SERVER: ";
			switch (error.value())
			{
			case PacketReceiveError::READ_HEADER: {
				out << "Error reading incoming packet header. (Code: " <<
					*error.arg<boost::system::error_code>() << ")";
			} break;
			case PacketReceiveError::READ_BODY: {
				out << "Error reading incoming packet body. (Code: " <<
					*error.arg<boost::system::error_code>() << ")";
			} break;
			case PacketReceiveError::BUF_EXCEED: {
				auto* info = error.arg<PacketReceiveBufExceedInfo>();
				out << "Error reading incoming packet body, "
					   "packet exceeds buffer size. (ID: "
					<< info->pktID << ", Size: " << info->pktSize << ")";
			} break;
			case PacketReceiveError::APPLY: {
				out << "Error applying incoming packet, invalid ID. (Got: "
					<< *error.arg<int>() << ")";
			} break;
			}
			conClient.sendPacket(S2CDisconnectPacket(out.str()));
			conClient.disconnect();
		}
	);
	conClient.setPacketSendErrorHandler(
		[&conClient](const PacketSendError& error){
			switch (error.value())
			{
			case PacketSendError::UNLISTED_PKT: {
				std::cout << "Error trying to send packet, "
						  << error.arg<PacketProfile>()->name << " was not registered." << std::endl;
			} break;
			case PacketSendError::WRITE_PKT: {
				std::cout << "Error writing outgoing packet. (Code: "
						  << *error.arg<boost::system::error_code>() << ")" << std::endl;
			} break;
			}
			conClient.disconnect();
		}
	);
}

void ConnectionHandler::onClientDisconnected(ConnectedClient& conClient)
{
	std::size_t pktHandlerCount = m_pktHandlers.size();
	for (std::size_t i = 0; i < pktHandlerCount; i++)
	{
		ServerPlayPacketHandler* pktHandler = m_pktHandlers[i];
		ConnectedClient& conClient1 = pktHandler->getConnection();
		if (&conClient1 == &conClient)
		{
			std::cout << pktHandler->getName() << " disconnected." << std::endl;
			delete pktHandler;
			m_pktHandlers.erase(m_pktHandlers.begin() + static_cast<int>(i));
			break;
		}
	}
}

void ConnectionHandler::sendPacketExcluding(const Packet& packet, ConnectedClient& exclude)
{
	auto connectedClients = m_server.getConnectedClients();
	for (ConnectedClient* conClient : connectedClients)
	{
		if (conClient != &exclude)
			conClient->sendPacket(packet);
	}
}

int main(int argc, char* argv[])
{
	try
	{
		std::ios_base::sync_with_stdio(false);

		std::cout << "Server is starting..." << std::endl;
		ServerConnection con(25565);

		ConnectionHandler conHandler(con);
		con.setConnectionListener(conHandler);
		con.start();

		std::cout << "Server is now listening for connections." << std::endl;

		con.join();
	}
	catch (std::exception& e)
	{
		std::cerr << "Exception: " << e.what() << "\n";
	}

	return 0;
}
