#pragma once

#include <string>

#include "../../Packet.hpp"
#include "../../PacketListener.hpp"
#include "../../ByteBuf.hpp"

class S2CChatMessagePacket : public Packet
{
public:
	DECL_PACKET_PROFILE();

	S2CChatMessagePacket(std::string message);
	S2CChatMessagePacket(ByteBuf& buf);

	ByteBuf serialize() const override;
	void apply(PacketListener& listener) const override;

	constexpr
	const std::string& getMessage() const
	{ return m_msg; }

private:
	std::string m_msg;
};
