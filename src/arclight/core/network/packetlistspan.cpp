#include "packetlistspan.hpp"

NETWORK_BEGIN

u32 PacketListSpan::find(const PacketProfile& profile)
{
	u32 i = 0;
	for (; i < m_pktProfileCount; i++)
	{
		if (&profile == m_pktProfiles[i])
			break;
	}
	return i;
}

NETWORK_END
