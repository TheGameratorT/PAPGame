#include "pkt_c2s_typestate.hpp"

#include "network/packetbuffer.hpp"

IMPL_PACKET(PKT_C2S_TypeState)

PKT_C2S_TypeState::PKT_C2S_TypeState(u32 charsTyped) :
	m_charsTyped(charsTyped)
{}

PKT_C2S_TypeState::PKT_C2S_TypeState(Network::PacketBuffer& buf) :
	m_charsTyped(buf.read<u32>())
{}

void PKT_C2S_TypeState::serialize(Network::PacketBuffer& buf) const
{
	buf.write<u32>(m_charsTyped);
}
