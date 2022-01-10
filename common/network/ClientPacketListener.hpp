#pragma once

#include "PacketListener.hpp"

#include "packet/s2c/S2CDisconnectPacket.hpp"
#include "packet/s2c/S2CChatMessagePacket.hpp"
#include "packet/s2c/S2CConnectReplyPacket.hpp"

class ClientPacketListener : public PacketListener
{
public:
	virtual void onDisconnect(const S2CDisconnectPacket& packet) = 0;
	virtual void onConnectReply(const S2CConnectReplyPacket& packet) = 0;
	virtual void onChatMessage(const S2CChatMessagePacket& packet) = 0;
};
