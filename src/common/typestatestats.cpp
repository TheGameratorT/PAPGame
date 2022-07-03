#include "typestatestats.hpp"

#include "network/packetbuffer.hpp"

void TypeStateStats::packetWrite(Network::PacketBuffer& buf) const
{
	buf.write(leadingPlayerID);
	buf.write(leadingPlayerTL);
	buf.write(leadingPlace);
	buf.write(inFrontPlayerID);
}

void TypeStateStats::packetRead(Network::PacketBuffer& buf)
{
	leadingPlayerID = buf.read<u32>();
	leadingPlayerTL = buf.read<u32>();
	leadingPlace = buf.read<u32>();
	inFrontPlayerID = buf.read<u32>();
}
