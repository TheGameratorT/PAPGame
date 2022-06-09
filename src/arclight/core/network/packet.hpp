#pragma once

#include <array>

#include "types.hpp"
#include "common/concepts.hpp"

#include "packetbuffer.hpp"

// TODO: UDP Packets?

#define DECL_PACKET() \
static const Network::PacketProfile profile; \
const Network::PacketProfile& getProfile() const override;

#define IMPL_PACKET(T) \
const Network::PacketProfile T::profile = { construct<T>, #T }; \
const Network::PacketProfile& T::getProfile() const { return profile; }

NETWORK_BEGIN

using PacketID = u32;

class Packet;

using PacketCtorT = Packet* (*)(PacketBuffer&);

struct PacketProfile
{
	PacketCtorT ctor;
	const char* name;
};

class Packet
{
public:
	virtual ~Packet() = default;

	virtual void serialize(PacketBuffer& buf) const = 0;
	[[nodiscard]] virtual const PacketProfile& getProfile() const = 0;

	template<class T> requires CC::BaseOf<Packet, T>
	static Packet* construct(PacketBuffer& buf)
	{ return new T(buf); }
};

template<class T>
concept PacketType = CC::BaseOf<Packet, T>;

NETWORK_END
