#pragma once

#include "network/packet.hpp"

#include <string>

class PKT_C2S_PlayerReady : public Network::Packet
{
public:
	DECL_PACKET()

	enum ReadyType
	{
		Ready = 0,
		NotReady = 1,
		GameReady = 2
	};

	explicit PKT_C2S_PlayerReady(ReadyType ready);

	explicit PKT_C2S_PlayerReady(Network::PacketBuffer& buf);
	void serialize(Network::PacketBuffer& buf) const override;

	[[nodiscard]] constexpr ReadyType getReady() const { return m_ready; }

private:
	ReadyType m_ready;
};
