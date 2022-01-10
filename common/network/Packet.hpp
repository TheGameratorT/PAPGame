#pragma once

#include <array>
#include <string>
#include <vector>
#include <cstdint>

#include "ByteBuf.hpp"
#include "PacketListener.hpp"

#define INVALID_PACKET_ID 0xFFFFFFFF

#define DECL_PACKET_PROFILE() \
static const PacketProfile profile; \
const PacketProfile& getProfile() const override;

#define IMPL_PACKET_PROFILE(T) \
const PacketProfile T::profile = { construct<T>, #T }; \
const PacketProfile& T::getProfile() const { return profile; }

class Packet;

using PacketConstructor = Packet* (*)(ByteBuf& buf);

struct PacketProfile {
	PacketConstructor constructor;
	const char* name;
};

namespace PacketUtils {
	template<typename ...P>
	static consteval auto createPacketTable()
	{
		return std::array<const PacketProfile*, sizeof...(P) + 1>{{
			&P::profile..., nullptr
		}};
	}

	std::uint_fast32_t getPacketProfileCount(const PacketProfile* const* profiles);

	constexpr
	const PacketProfile* getPacketProfileByID(const PacketProfile* const* profiles, std::uint_fast32_t id)
	{ return profiles[id]; }

	std::uint_fast32_t getPacketProfileID(const PacketProfile* profile, const PacketProfile* const* profiles);
}

class Packet
{
public:
	virtual ~Packet() = default;

	virtual ByteBuf serialize() const = 0;
	virtual void apply(PacketListener& listener) const = 0;
	virtual const PacketProfile& getProfile() const = 0;

	template<class T> requires std::is_base_of_v<Packet, T>
	static Packet* construct(ByteBuf& buf)
	{ return new T(buf); }

protected:
	Packet() = default;
};
