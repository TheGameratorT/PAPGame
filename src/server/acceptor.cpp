#include "acceptor.hpp"

#include "gameinfo.hpp"
#include "network/packettable.hpp"
#include "network/packet/pkt_c2s_handshake.hpp"
#include "network/packet/pkt_s2c_handshake.hpp"

#include "util/log.hpp"

namespace Net = Network;

Acceptor::Acceptor() :
	m_server(new Net::Server())
{}

Acceptor::~Acceptor()
{
	delete m_server;
}

void Acceptor::start(u16 port)
{
	m_server->start(port);

	m_server->setConnectionListener([this](Net::ConnectedClientPtr other){
		other->setIncomingPackets(PacketTable::packetsC2S);
		other->setOutgoingPackets(PacketTable::packetsS2C);

		other->setIncomingPacketListener([this, other](Net::PacketID id, const Net::Packet& basePacket){
			if (id != PacketTable::idOfC2S<PKT_C2S_Handshake>())
			{
				// Client tried to send packet before handshaking
				// TODO: Send message to client saying he tried to handshake more than once
				other->close();
				return;
			}

			const auto& packet = static_cast<const PKT_C2S_Handshake&>(basePacket);

			if (packet.getMagic() != GameInfo::handshakeMagic)
			{
				// Client tried to send invalid packet
				// Do not send message because it might not be our client
				other->close();
				return;
			}

			if (packet.getVersion() != GameInfo::version)
			{
				// Client tried to send invalid packet
				// TODO: Send message to client saying there is a version mismatch
				other->close();
				return;
			}

			// Now that an actual recognized connection was made we can set a proper error handler
			other->setErrorHandler([](const Net::ConnectionError& error){
				std::ostringstream out;
				switch (error.value())
				{
				case Net::ConnectionError::ConnectionFailed: {
					out << "Error connecting to server. (Code: " << error.getCode() << ")";
				} break;
				case Net::ConnectionError::SendPacketUnlisted: {
					out << "Error trying to send packet, " <<
						error.arg<Net::PacketProfile>()->name << " was not registered.";
				} break;
				case Net::ConnectionError::SendPacketWrite: {
					out << "Error writing outgoing packet. (Code: " << error.getCode() << ")";
				} break;
				case Net::ConnectionError::ReadPacketHeader: {
					out << "Error reading incoming packet header. (Code: " << error.getCode() << ")";
				} break;
				case Net::ConnectionError::ReadPacketBody: {
					out << "Error reading incoming packet body. (Code: " << error.getCode() << ")";
				} break;
				case Net::ConnectionError::ReadPacketInvalid: {
					out << "Error identifying incoming packet. (Packet ID: " << *error.arg<u32>() << ")";
				} break;
				case Net::ConnectionError::ReadPacketBufferExceeded: {
					auto* info = error.arg<PacketReceiveBufExceedInfo>();
					out << "Error reading incoming packet body, "
						   "packet exceeds buffer size. (ID: "
						<< info->pktID << ", Size: " << info->pktSize << ")";
				} break;
				}
				// TODO: Send read error to client
				Log::info("Server", out.str());
			});

			other->sendPacket<PKT_S2C_Handshake>(GameInfo::handshakeMagic);

			U8String playerName = packet.getPlayerName();
			if (playerName.size() > 16)
			{
				// TODO: Send message to client saying the player name is too long
				other->close();
				return;
			}

			// TODO: Check for conflicting nicknames

			// TODO: Limit max of 8 players

			if (m_clientAcceptedCallback)
				m_clientAcceptedCallback(std::make_unique<Player>(other, playerName));
		});

		other->setErrorHandler([&](const Net::ConnectionError& error){
			// Because no handshake has been done yet, we can't
			// verify if the connection is legitimate, in that case
			// just close the connection if an error occurs.
			other->close();
			return;
		});
	});
}

void Acceptor::stop()
{
	m_server->stop();
}

bool Acceptor::hasStopped() const
{
	return m_server->hasStopped();
}
