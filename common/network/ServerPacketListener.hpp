#pragma once

#include "PacketListener.hpp"

#include "packet/c2s/C2SDisconnectPacket.hpp"
#include "packet/c2s/C2SClientInfoPacket.hpp"
#include "packet/c2s/C2SChatMessagePacket.hpp"

class ServerPacketListener : public PacketListener
{
public:
	virtual void onDisconnect(const C2SDisconnectPacket& packet) = 0;
	virtual void onClientInfo(const C2SClientInfoPacket& packet) = 0;
	virtual void onChatMessage(const C2SChatMessagePacket& packet) = 0;
};
