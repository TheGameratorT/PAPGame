#pragma once

#include <string>

#include "../../Packet.hpp"
#include "../../PacketListener.hpp"

class S2CDisconnectPacket : public Packet
{
public:
	DECL_PACKET_PROFILE();

	S2CDisconnectPacket(std::string reason);
	S2CDisconnectPacket(ByteBuf& buf);

	ByteBuf serialize() const override;
	void apply(PacketListener& listener) const override;

	constexpr
	const std::string& getReason() const
	{ return m_reason; }

private:
	std::string m_reason;
};
