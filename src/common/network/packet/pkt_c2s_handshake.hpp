#pragma once

#include "network/packet.hpp"
#include "locale/unicodestring.hpp"

#include <string>

class PKT_C2S_Handshake : public Network::Packet
{
public:
	DECL_PACKET()

	explicit PKT_C2S_Handshake(std::string magic, u32 gameVersion, U8String playerName);

	explicit PKT_C2S_Handshake(Network::PacketBuffer& buf);
	void serialize(Network::PacketBuffer& buf) const override;

	[[nodiscard]] constexpr const std::string& getMagic() const { return m_magic; }
	[[nodiscard]] constexpr u32 getVersion() const { return m_gameVersion; }
	[[nodiscard]] constexpr const U8String& getPlayerName() const { return m_playerName; }

private:
	std::string m_magic;
	u32 m_gameVersion;
	U8String m_playerName;
};
