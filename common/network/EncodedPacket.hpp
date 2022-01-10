#pragma once

#include <deque>
#include <memory>

#include "Packet.hpp"

struct EncodedPacketHeader
{
	std::uint32_t id;
	std::uint32_t bodySize;
};

class EncodedPacket
{
public:
	static constexpr const std::size_t headerSize = sizeof(EncodedPacketHeader);

	EncodedPacket() = default;
	~EncodedPacket() = default;

	EncodedPacket(const EncodedPacket& other);
	EncodedPacket(EncodedPacket&& other) = default;

	EncodedPacket& operator=(const EncodedPacket& other) { return *this = EncodedPacket(other); }
	EncodedPacket& operator=(EncodedPacket&& other) = default;

	static EncodedPacket encode(const Packet& packet, std::uint_fast32_t id);
	static EncodedPacket createReader(std::size_t bufferSize);

	inline std::uint_fast32_t id() const {
		return reinterpret_cast<const EncodedPacketHeader*>(m_data.get())->id;
	}

	inline std::uint_fast32_t bodySize() const {
		return reinterpret_cast<const EncodedPacketHeader*>(m_data.get())->bodySize;
	}

	inline char* data() { return m_data.get(); }
	inline const char* data() const { return m_data.get(); }

	inline char* body() { return &m_data[headerSize]; }
	inline const char* body() const { return &m_data[headerSize]; }

	constexpr std::size_t size() const { return m_dataSize; }

private:
	EncodedPacket(std::unique_ptr<char[]> data, std::size_t dataSize);

	std::unique_ptr<char[]> m_data;
	std::size_t m_dataSize;
};

using EncodedPacketQueue = std::deque<std::unique_ptr<EncodedPacket>>;
