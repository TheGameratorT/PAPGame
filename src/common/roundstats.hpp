#pragma once

#include <vector>

#include "types.hpp"
#include "network/ipacketserializeable.hpp"

class RoundStats : public Network::IPacketSerializeable
{
public:
	struct PInfo
	{
		u32 playerID;
		u32 place;
		u32 points;
	};

	std::vector<PInfo> playerInfo;

	void packetWrite(Network::PacketBuffer& buf) const override;
	void packetRead(Network::PacketBuffer& buf) override;
};
