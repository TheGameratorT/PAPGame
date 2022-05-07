#pragma once

#include <array>

#include "common.hpp"
#include "packet.hpp"

NETWORK_BEGIN

namespace PacketList
{
	template<PacketType ...P>
	static consteval auto create()
	{ return std::array<const PacketProfile*, sizeof...(P)>{{ &P::profile... }}; }
}

NETWORK_END
