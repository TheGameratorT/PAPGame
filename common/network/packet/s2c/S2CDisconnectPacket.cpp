#include "S2CDisconnectPacket.hpp"

#include <utility>

#include "../../ClientPacketListener.hpp"

IMPL_PACKET_PROFILE(S2CDisconnectPacket);

S2CDisconnectPacket::S2CDisconnectPacket(std::string reason) :
	m_reason(std::move(reason))
{}

S2CDisconnectPacket::S2CDisconnectPacket(ByteBuf& buf)
{
	m_reason = buf.readString();
}

ByteBuf S2CDisconnectPacket::serialize() const
{
	ByteBuf buf;
	buf.writeString(m_reason);
	return buf;
}

void S2CDisconnectPacket::apply(PacketListener& listener) const
{
    reinterpret_cast<ClientPacketListener&>(listener).onDisconnect(*this);
}
