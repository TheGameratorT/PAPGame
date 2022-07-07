#pragma once

#include "typestatestats.hpp"
#include "network/packet.hpp"

#include <string>

class PKT_S2C_TypeState : public Network::Packet
{
public:
	DECL_PACKET()

	// isReady is false and story ID is 0 if sending stats
	PKT_S2C_TypeState(bool isReady, u32 storyID, TypeStateStats* stats);
	~PKT_S2C_TypeState() override;

	explicit PKT_S2C_TypeState(Network::PacketBuffer& buf);
	void serialize(Network::PacketBuffer& buf) const override;

	[[nodiscard]] constexpr bool getIsReady() const { return m_isReady; }
	[[nodiscard]] constexpr u32 getStoryID() const { return m_storyID; }
	[[nodiscard]] constexpr const TypeStateStats* getStats() const { return m_stats; }

private:
	bool m_isReady;
	u32 m_storyID;
	TypeStateStats* m_stats;
};
