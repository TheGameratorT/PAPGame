#pragma once

#include "network/packet.hpp"

#include <string>

class PKT_S2C_LobbyMsg : public Network::Packet
{
public:
	DECL_PACKET()

	explicit PKT_S2C_LobbyMsg(U8String msg);

	explicit PKT_S2C_LobbyMsg(Network::PacketBuffer& buf);
	void serialize(Network::PacketBuffer& buf) const override;

	[[nodiscard]] constexpr const U8String& getMsg() const { return m_msg; }

private:
	U8String m_msg;
};
