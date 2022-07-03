#pragma once

#include "types.hpp"
#include "network/ipacketserializeable.hpp"

class TypeStateStats : public Network::IPacketSerializeable
{
public:
	u32 leadingPlayerID = 0; // leading player id
	u32 leadingPlayerTL = 0; // leading player typed letters
	u32 leadingPlace = 0; // player round place 1st, 2nd, etc
	u32 inFrontPlayerID = 0; // player that is in front of you (you if you are first place)

	void packetWrite(Network::PacketBuffer& buf) const override;
	void packetRead(Network::PacketBuffer& buf) override;
};
