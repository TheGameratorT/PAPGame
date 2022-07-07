#pragma once

#include "network/packetlistspan.hpp"

#define PacketTable_DEFINE_ID(name, id) if (std::is_same_v<name, T>) return id;

#define PKT_CALL(type, func) func(static_cast<const type&>(packet))
#define PKT_C2S_JUMP(type, func) case PacketTable::idOfC2S<type>(): PKT_CALL(type, func); break
#define PKT_S2C_JUMP(type, func) case PacketTable::idOfS2C<type>(): PKT_CALL(type, func); break

class PKT_C2S_Handshake;
class PKT_C2S_LobbyMsg;
class PKT_C2S_PlayerReady;
class PKT_C2S_TypeState;
class PKT_C2S_PongMove;

class PKT_S2C_Handshake;
class PKT_S2C_Disconnect;
class PKT_S2C_LobbyData;
class PKT_S2C_LobbyMsg;
class PKT_S2C_PlayerState;
class PKT_S2C_StartGame;
class PKT_S2C_TypeState;
class PKT_S2C_TypeEnd;
class PKT_S2C_GameEnd;
class PKT_S2C_PongStart;
class PKT_S2C_PongMove;
class PKT_S2C_PongPoint;
class PKT_S2C_PongEnd;

namespace PacketTable
{
	extern Network::PacketListSpan packetsC2S;
	extern Network::PacketListSpan packetsS2C;

	template<class T>
	constexpr u32 idOfC2S()
	{
		PacketTable_DEFINE_ID(PKT_C2S_Handshake, 0)
		PacketTable_DEFINE_ID(PKT_C2S_LobbyMsg, 1)
		PacketTable_DEFINE_ID(PKT_C2S_PlayerReady, 2)
		PacketTable_DEFINE_ID(PKT_C2S_TypeState, 3)
		PacketTable_DEFINE_ID(PKT_C2S_PongMove, 4)
		return -1;
	}

	template<class T>
	constexpr u32 idOfS2C()
	{
		PacketTable_DEFINE_ID(PKT_S2C_Handshake, 0)
		PacketTable_DEFINE_ID(PKT_S2C_Disconnect, 1)
		PacketTable_DEFINE_ID(PKT_S2C_LobbyData, 2)
		PacketTable_DEFINE_ID(PKT_S2C_LobbyMsg, 3)
		PacketTable_DEFINE_ID(PKT_S2C_PlayerState, 4)
		PacketTable_DEFINE_ID(PKT_S2C_StartGame, 5)
		PacketTable_DEFINE_ID(PKT_S2C_TypeState, 6)
		PacketTable_DEFINE_ID(PKT_S2C_TypeEnd, 7)
		PacketTable_DEFINE_ID(PKT_S2C_GameEnd, 8)
		PacketTable_DEFINE_ID(PKT_S2C_PongStart, 9)
		PacketTable_DEFINE_ID(PKT_S2C_PongMove, 10)
		PacketTable_DEFINE_ID(PKT_S2C_PongPoint, 11)
		PacketTable_DEFINE_ID(PKT_S2C_PongEnd, 12)
		return -1;
	}
}
