#include "S2CConnectReplyPacket.hpp"

#include <utility>

#include "../../ClientPacketListener.hpp"

IMPL_PACKET_PROFILE(S2CConnectReplyPacket);

S2CConnectReplyPacket::S2CConnectReplyPacket(bool accepted) :
	m_accepted(accepted)
{}

S2CConnectReplyPacket::S2CConnectReplyPacket(bool accepted, std::string denyReason) :
	m_accepted(accepted),
	m_denyReason(std::move(denyReason))
{}

S2CConnectReplyPacket::S2CConnectReplyPacket(ByteBuf& buf)
{
	m_accepted = buf.readBool();
	if (!m_accepted)
		m_denyReason = buf.readString();
}

ByteBuf S2CConnectReplyPacket::serialize() const
{
	ByteBuf buf;
	buf.writeBool(m_accepted);
	if (!m_accepted)
		buf.writeString(m_denyReason);
	return buf;
}

void S2CConnectReplyPacket::apply(PacketListener& listener) const
{
	reinterpret_cast<ClientPacketListener&>(listener).onConnectReply(*this);
}
