#pragma once

#include "network/packet.hpp"

#include <string>

class PKT_C2S_LobbyMsg : public Network::Packet
{
public:
	DECL_PACKET()

	explicit PKT_C2S_LobbyMsg(U8String msg);

	explicit PKT_C2S_LobbyMsg(Network::PacketBuffer& buf);
	void serialize(Network::PacketBuffer& buf) const override;

	[[nodiscard]] constexpr const U8String& getMsg() const { return m_msg; }

private:
	U8String m_msg;
};
