#pragma once

#include "network/packet.hpp"

#include <string>

class PKT_S2C_Handshake : public Network::Packet
{
public:
	DECL_PACKET()

	explicit PKT_S2C_Handshake(std::string magic);

	explicit PKT_S2C_Handshake(Network::PacketBuffer& buf);
	void serialize(Network::PacketBuffer& buf) const override;

	[[nodiscard]] constexpr const std::string& getMagic() const
	{ return m_magic; }

private:
	std::string m_magic;
};
