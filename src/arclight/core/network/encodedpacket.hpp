#pragma once

#include <memory>

#include "types.hpp"
#include "common.hpp"
#include "packet.hpp"

NETWORK_BEGIN

struct EncodedPacketHeader
{
	u32 id;
	u32 bodySize;
};

class EncodedPacket
{
public:
	static constexpr const std::size_t headerSize = sizeof(EncodedPacketHeader);

	EncodedPacket();

	static EncodedPacket encode(const Packet& packet, u32 id);
	static EncodedPacket createReader(std::size_t bufferSize);

	[[nodiscard]] inline u32 id() const
	{ return reinterpret_cast<const EncodedPacketHeader*>(m_data.get())->id; }

	[[nodiscard]] inline u32 bodySize() const
	{ return reinterpret_cast<const EncodedPacketHeader*>(m_data.get())->bodySize; }

	inline char* data() { return m_data.get(); }
	[[nodiscard]] inline const char* data() const { return m_data.get(); }

	inline char* body() { return &m_data[headerSize]; }
	[[nodiscard]] inline const char* body() const { return &m_data[headerSize]; }

	[[nodiscard]] constexpr std::size_t size() const { return m_dataSize; }

private:
	EncodedPacket(std::unique_ptr<char[]> data, std::size_t dataSize);

	std::unique_ptr<char[]> m_data;
	std::size_t m_dataSize;
};

NETWORK_END
