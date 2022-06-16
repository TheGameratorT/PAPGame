#pragma once

#include "network/packetlistspan.hpp"

#define PacketTable_DEFINE_ID(name, id) if (std::is_same_v<name, T>) return id;

class PKT_C2S_Handshake;
class PKT_C2S_LobbyMsg;

class PKT_S2C_Handshake;
class PKT_S2C_Disconnect;
class PKT_S2C_LobbyData;
class PKT_S2C_LobbyMsg;

namespace PacketTable
{
	extern Network::PacketListSpan packetsC2S;
	extern Network::PacketListSpan packetsS2C;

	template<class T>
	constexpr u32 idOfC2S()
	{
		PacketTable_DEFINE_ID(PKT_C2S_Handshake, 0)
		PacketTable_DEFINE_ID(PKT_C2S_LobbyMsg, 1)
		return -1;
	}

	template<class T>
	constexpr u32 idOfS2C()
	{
		PacketTable_DEFINE_ID(PKT_S2C_Handshake, 0)
		PacketTable_DEFINE_ID(PKT_S2C_Disconnect, 1)
		PacketTable_DEFINE_ID(PKT_S2C_LobbyData, 2)
		PacketTable_DEFINE_ID(PKT_S2C_LobbyMsg, 3)
		return -1;
	}
}
