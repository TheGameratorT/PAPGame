#pragma once

#include "roundstats.hpp"
#include "network/packet.hpp"
#include "network/ipacketserializeable.hpp"

#include <string>

class PKT_S2C_TypeEnd : public Network::Packet
{
public:
	DECL_PACKET()

	// if wasLastRound is true then finalStats must not be nullptr
	PKT_S2C_TypeEnd(bool wasLastRound, RoundStats* stats, RoundStats* finalStats);
	~PKT_S2C_TypeEnd() override;

	explicit PKT_S2C_TypeEnd(Network::PacketBuffer& buf);
	void serialize(Network::PacketBuffer& buf) const override;

	[[nodiscard]] constexpr bool getWasLastRound() const { return m_wasLastRound; }
	[[nodiscard]] constexpr const RoundStats* getStats() const { return m_stats; }
	[[nodiscard]] constexpr const RoundStats* getFinalStats() const { return m_finalStats; }

private:
	bool m_wasLastRound;
	RoundStats* m_stats;
	RoundStats* m_finalStats;
};
