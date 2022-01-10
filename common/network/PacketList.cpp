#include "PacketList.hpp"

#include "network/packet/c2s/C2SChatMessagePacket.hpp"
#include "network/packet/c2s/C2SClientInfoPacket.hpp"
#include "network/packet/c2s/C2SDisconnectPacket.hpp"
#include "network/packet/s2c/S2CDisconnectPacket.hpp"
#include "network/packet/s2c/S2CConnectReplyPacket.hpp"
#include "network/packet/s2c/S2CChatMessagePacket.hpp"

static constexpr auto C2SPacketsA =
PacketUtils::createPacketTable<
	C2SDisconnectPacket,
	C2SClientInfoPacket,
	C2SChatMessagePacket
>();

static constexpr auto S2CPacketsA =
PacketUtils::createPacketTable<
	S2CDisconnectPacket,
	S2CConnectReplyPacket,
	S2CChatMessagePacket
>();

namespace PacketList
{
	const PacketProfile* const* C2SPackets = C2SPacketsA.data();
	const PacketProfile* const* S2CPackets = S2CPacketsA.data();
}
