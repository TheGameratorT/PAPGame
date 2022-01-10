#include "S2CChatMessagePacket.hpp"

#include <utility>

#include "../../ClientPacketListener.hpp"

IMPL_PACKET_PROFILE(S2CChatMessagePacket);

S2CChatMessagePacket::S2CChatMessagePacket(std::string message) :
	m_msg(std::move(message))
{}

S2CChatMessagePacket::S2CChatMessagePacket(ByteBuf& buf)
{
	m_msg = buf.readString();
}

ByteBuf S2CChatMessagePacket::serialize() const
{
	ByteBuf buf;
	buf.writeString(m_msg);
	return buf;
}

void S2CChatMessagePacket::apply(PacketListener& listener) const
{
    reinterpret_cast<ClientPacketListener&>(listener).onChatMessage(*this);
}
