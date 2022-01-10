#pragma once

#include <string>
#include <cstdint>

#include "../../Packet.hpp"
#include "../../PacketListener.hpp"
#include "../../ByteBuf.hpp"

class S2CConnectReplyPacket : public Packet
{
public:
	DECL_PACKET_PROFILE();

	S2CConnectReplyPacket(bool accepted);
	S2CConnectReplyPacket(bool accepted, std::string denyReason);
	S2CConnectReplyPacket(ByteBuf& buf);

	ByteBuf serialize() const override;
	void apply(PacketListener& listener) const override;

	constexpr
	bool getAccepted() const
	{ return m_accepted; }

	constexpr
	const std::string& getDenyReason() const
	{ return m_denyReason; }

private:
	bool m_accepted;
	std::string m_denyReason;
};
