#pragma once

#include <string>

#include "../../Packet.hpp"
#include "../../PacketListener.hpp"
#include "../../ByteBuf.hpp"

class C2SDisconnectPacket : public Packet
{
public:
	DECL_PACKET_PROFILE();

	C2SDisconnectPacket();
	C2SDisconnectPacket(ByteBuf& buf);

	ByteBuf serialize() const override;
	void apply(PacketListener& listener) const override;
};
