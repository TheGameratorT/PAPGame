#pragma once

#include "types.hpp"
#include "common.hpp"
#include "util/concepts.hpp"

#include <string>
#include <vector>

NETWORK_BEGIN

class PacketBuffer
{
public:
	PacketBuffer();
	explicit PacketBuffer(std::vector<char> data);

	void reserve(std::size_t size);
	void seek(std::size_t size);
	[[nodiscard]] std::size_t readableBytes() const;

	constexpr
	std::vector<char>& getData()
	{ return m_data; }

	[[nodiscard]] constexpr
	const std::vector<char>& getData() const
	{ return m_data; }

	template<Integral T>
	T read()
	{
		T val = *reinterpret_cast<T*>(&m_data[m_cursor]);
		m_cursor += sizeof(T);
		return val;
	}

	template<Integral T>
	void write(T val)
	{
		std::size_t writeSize = sizeof(T);
		expand(writeSize);
		*reinterpret_cast<T*>(&m_data[m_cursor]) = val;
		m_cursor += writeSize;
	}

	inline bool readBool()
	{ return read<u8>(); }

	inline void writeBool(bool val)
	{ write<u8>(val); }

	std::string readString();
	void writeString(const std::string& str);

private:
	void expand(std::size_t size);

	std::vector<char> m_data;
	std::size_t m_cursor;
};

NETWORK_END
