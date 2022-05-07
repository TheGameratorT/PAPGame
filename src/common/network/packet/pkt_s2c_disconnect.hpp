#pragma once

#include "network/packet.hpp"

#include <string>

class PKT_S2C_Disconnect : public Network::Packet
{
public:
	DECL_PACKET()

	explicit PKT_S2C_Disconnect(std::string message);

	explicit PKT_S2C_Disconnect(Network::PacketBuffer& buf);
	void serialize(Network::PacketBuffer& buf) const override;

	[[nodiscard]] constexpr const std::string& getMessage() const
	{ return m_message; }

private:
	std::string m_message;
};
