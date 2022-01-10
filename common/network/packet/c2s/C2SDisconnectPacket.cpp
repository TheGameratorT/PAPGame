#include "C2SDisconnectPacket.hpp"

#include "../../ServerPacketListener.hpp"

IMPL_PACKET_PROFILE(C2SDisconnectPacket);

C2SDisconnectPacket::C2SDisconnectPacket() {}
C2SDisconnectPacket::C2SDisconnectPacket(ByteBuf& buf) {}

ByteBuf C2SDisconnectPacket::serialize() const
{
	return {};
}

void C2SDisconnectPacket::apply(PacketListener& listener) const
{
    reinterpret_cast<ServerPacketListener&>(listener).onDisconnect(*this);
}
