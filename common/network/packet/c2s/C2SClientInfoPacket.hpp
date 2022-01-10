#pragma once

#include <string>
#include <cstdint>

#include "../../Packet.hpp"
#include "../../PacketListener.hpp"
#include "../../ByteBuf.hpp"

class C2SClientInfoPacket : public Packet
{
public:
	DECL_PACKET_PROFILE();

	C2SClientInfoPacket(std::uint_fast32_t protocol, std::string message);
	C2SClientInfoPacket(ByteBuf& buf);

	ByteBuf serialize() const override;
	void apply(PacketListener& listener) const override;

	constexpr
	std::uint_fast32_t getProtocol() const
	{ return m_protocol; }

	constexpr
	const std::string& getName() const
	{ return m_name; }

private:
	std::uint_fast32_t m_protocol;
	std::string m_name;
};
