#include "packettable.hpp"

#include "network/packetlist.hpp"

#include "packet/pkt_c2s_handshake.hpp"

#include "packet/pkt_s2c_handshake.hpp"
#include "packet/pkt_s2c_disconnect.hpp"

namespace PacketTable
{
	static auto ApacketsC2S = Network::PacketList::create<
		PKT_C2S_Handshake
	>();

	static auto ApacketsS2C = Network::PacketList::create<
		PKT_S2C_Handshake,
		PKT_S2C_Disconnect
	>();

	Network::PacketListSpan packetsC2S = { ApacketsC2S.data(), ApacketsC2S.size() };
	Network::PacketListSpan packetsS2C = { ApacketsS2C.data(), ApacketsS2C.size() };
}
