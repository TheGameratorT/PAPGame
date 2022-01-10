#include "Packet.hpp"

namespace PacketUtils
{
	std::uint_fast32_t getPacketProfileCount(const PacketProfile* const* profiles)
	{
		std::uint_fast32_t i = 0;
		while (profiles[i] != nullptr) { i++; }
		return i;
	}

	std::uint_fast32_t getPacketProfileID(const PacketProfile* profile, const PacketProfile* const* profiles)
	{
		const PacketProfile* iprofile;
		std::uint_fast32_t i = 0;
		while ((iprofile = profiles[i]) != nullptr)
		{
			if (profile == iprofile)
				return i;
			i++;
		}
		return INVALID_PACKET_ID;
	}
}
