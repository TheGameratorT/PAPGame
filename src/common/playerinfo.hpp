#pragma once

#include <string>

#include "types.hpp"
#include "locale/unicodestring.hpp"

#include "network/packetbuffer.hpp"
#include "network/ipacketserializeable.hpp"

class PlayerInfo : public Network::IPacketSerializeable
{
public:
	u32 id;
	U8String name;
	u32 team;
	bool ready;

	void packetWrite(Network::PacketBuffer& buf) const override;
	void packetRead(Network::PacketBuffer& buf) override;
};
