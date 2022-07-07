#pragma once

#include "roundstats.hpp"
#include "network/packet.hpp"

class PKT_S2C_GameEnd : public Network::Packet
{
public:
	DECL_PACKET()

	explicit PKT_S2C_GameEnd(RoundStats* stats);
	~PKT_S2C_GameEnd() override;

	explicit PKT_S2C_GameEnd(Network::PacketBuffer& buf);
	void serialize(Network::PacketBuffer& buf) const override;

	[[nodiscard]] constexpr const RoundStats* getStats() const { return m_stats; }

private:
	RoundStats* m_stats;
};
