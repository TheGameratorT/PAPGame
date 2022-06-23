#include "packettable.hpp"

#include "network/packetlist.hpp"

#include "packet/pkt_c2s_handshake.hpp"
#include "packet/pkt_c2s_lobbymsg.hpp"
#include "packet/pkt_c2s_playerready.hpp"

#include "packet/pkt_s2c_handshake.hpp"
#include "packet/pkt_s2c_disconnect.hpp"
#include "packet/pkt_s2c_lobbydata.hpp"
#include "packet/pkt_s2c_lobbymsg.hpp"
#include "packet/pkt_s2c_playerstate.hpp"
#include "packet/pkt_s2c_startgame.hpp"

namespace PacketTable
{
	static auto ApacketsC2S = Network::PacketList::create<
		PKT_C2S_Handshake,
		PKT_C2S_LobbyMsg,
		PKT_C2S_PlayerReady
	>();

	static auto ApacketsS2C = Network::PacketList::create<
		PKT_S2C_Handshake,
		PKT_S2C_Disconnect,
		PKT_S2C_LobbyData,
		PKT_S2C_LobbyMsg,
		PKT_S2C_PlayerState,
		PKT_S2C_StartGame
	>();

	Network::PacketListSpan packetsC2S = { ApacketsC2S.data(), ApacketsC2S.size() };
	Network::PacketListSpan packetsS2C = { ApacketsS2C.data(), ApacketsS2C.size() };
}
