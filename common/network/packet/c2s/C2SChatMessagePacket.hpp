#pragma once

#include <string>

#include "../../Packet.hpp"
#include "../../PacketListener.hpp"
#include "../../ByteBuf.hpp"

class C2SChatMessagePacket : public Packet
{
public:
	DECL_PACKET_PROFILE();

	C2SChatMessagePacket(std::string message);
	C2SChatMessagePacket(ByteBuf& buf);

	ByteBuf serialize() const override;
	void apply(PacketListener& listener) const override;

	constexpr
	const std::string& getMessage() const
	{ return m_msg; }

private:
	std::string m_msg;
};
