#pragma once

#include <array>

#include "common.hpp"
#include "packet.hpp"

NETWORK_BEGIN

class PacketListSpan
{
public:
	constexpr PacketListSpan() :
		m_pktProfiles(nullptr),
		m_pktProfileCount(0)
	{}

	constexpr PacketListSpan(const PacketProfile* const* profiles, u32 profileCount) :
		m_pktProfiles(profiles),
		m_pktProfileCount(profileCount)
	{}

	template<typename T, std::size_t S>
	constexpr PacketListSpan(const std::array<T, S>& array) :
		PacketListSpan(array.data(), S)
	{}

	constexpr const PacketProfile& operator[](u32 i) const
	{ return *m_pktProfiles[i]; }

	u32 find(const PacketProfile& profile);

	[[nodiscard]] constexpr u32 size() const
	{ return m_pktProfileCount; }

private:
	const PacketProfile* const* m_pktProfiles;
	u32 m_pktProfileCount;
};

NETWORK_END
