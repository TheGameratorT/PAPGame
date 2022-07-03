#include "roundstats.hpp"

#include "network/packetbuffer.hpp"

void RoundStats::packetWrite(Network::PacketBuffer& buf) const
{
	buf.write<u32>(playerInfo.size());
	for (auto& p : playerInfo)
	{
		buf.write(p.playerID);
		buf.write(p.place);
		buf.write(p.points);
	}
}

void RoundStats::packetRead(Network::PacketBuffer& buf)
{
	u32 pCount = buf.read<u32>();
	playerInfo.resize(pCount);
	for (auto& p : playerInfo)
	{
		p.playerID = buf.read<u32>();
		p.place = buf.read<u32>();
		p.points = buf.read<u32>();
	}
}
