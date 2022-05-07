#pragma once

#include "network/packet.hpp"

#include <string>

class PKT_C2S_Handshake : public Network::Packet
{
public:
	DECL_PACKET()

	explicit PKT_C2S_Handshake(std::string magic, int gameVersion);

	explicit PKT_C2S_Handshake(Network::PacketBuffer& buf);
	void serialize(Network::PacketBuffer& buf) const override;

	[[nodiscard]] constexpr const std::string& getMagic() const
	{ return m_magic; }

	[[nodiscard]] constexpr int getVersion() const
	{ return m_gameVersion; }

private:
	std::string m_magic;
	int m_gameVersion;
};
