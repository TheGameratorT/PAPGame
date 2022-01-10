#include "C2SChatMessagePacket.hpp"

#include <utility>

#include "../../ServerPacketListener.hpp"

IMPL_PACKET_PROFILE(C2SChatMessagePacket);

C2SChatMessagePacket::C2SChatMessagePacket(std::string message) :
	m_msg(std::move(message))
{}

C2SChatMessagePacket::C2SChatMessagePacket(ByteBuf& buf)
{
	m_msg = buf.readString();
}

ByteBuf C2SChatMessagePacket::serialize() const
{
	ByteBuf buf;
	buf.writeString(m_msg);
	return buf;
}

void C2SChatMessagePacket::apply(PacketListener& listener) const
{
	reinterpret_cast<ServerPacketListener&>(listener).onChatMessage(*this);
}
