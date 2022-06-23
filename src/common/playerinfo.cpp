#include "playerinfo.hpp"

void PlayerInfo::packetWrite(Network::PacketBuffer& buf) const
{
	buf.write<u32>(id);
	buf.writeU8String(name);
	buf.write<u32>(team);
	buf.writeBool(ready);
}

void PlayerInfo::packetRead(Network::PacketBuffer& buf)
{
	id = buf.read<u32>();
	name = buf.readU8String();
	team = buf.read<u32>();
	ready = buf.readBool();
}
