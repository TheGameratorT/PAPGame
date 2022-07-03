#pragma once

#include "common.hpp"

NETWORK_BEGIN

class PacketBuffer;

class IPacketSerializeable
{
public:
	virtual void packetWrite(PacketBuffer& buf) const = 0;
	virtual void packetRead(PacketBuffer& buf) = 0;
};

NETWORK_END
