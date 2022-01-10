#include "C2SClientInfoPacket.hpp"

#include <utility>

#include "../../ServerPacketListener.hpp"

IMPL_PACKET_PROFILE(C2SClientInfoPacket);

C2SClientInfoPacket::C2SClientInfoPacket(std::uint_fast32_t protocol, std::string name) :
	m_protocol(protocol),
	m_name(std::move(name))
{}

C2SClientInfoPacket::C2SClientInfoPacket(ByteBuf& buf)
{
	m_protocol = buf.read<std::uint32_t>();
	m_name = buf.readString();
}

ByteBuf C2SClientInfoPacket::serialize() const
{
	ByteBuf buf;
	buf.write<std::uint32_t>(m_protocol);
	buf.writeString(m_name);
	return buf;
}

void C2SClientInfoPacket::apply(PacketListener& listener) const
{
	reinterpret_cast<ServerPacketListener&>(listener).onClientInfo(*this);
}
