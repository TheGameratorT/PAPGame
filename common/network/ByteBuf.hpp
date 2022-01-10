#pragma once

#include <string>
#include <vector>
#include <cstdint>

class ByteBuf
{
public:
	ByteBuf();
	ByteBuf(std::vector<char> data);

	void reserve(std::size_t size);
	void seek(std::size_t size);
	std::size_t readableBytes() const;

	constexpr
	std::vector<char>& getData()
	{ return m_data; }

	constexpr
	const std::vector<char>& getData() const
	{ return m_data; }

	template<typename T>
	T read()
	{
		static_assert(std::is_integral<T>::value, "Integral required.");
		T val = *reinterpret_cast<T*>(&m_data[m_cursor]);
		m_cursor += sizeof(T);
		return val;
	}

	template<typename T>
	void write(T val)
	{
		static_assert(std::is_integral<T>::value, "Integral required.");
		std::size_t writeSize = sizeof(T);
		expand(writeSize);
		*reinterpret_cast<T*>(&m_data[m_cursor]) = val;
		m_cursor += writeSize;
	}

	inline bool readBool()
	{ return read<std::uint8_t>(); }

	inline void writeBool(bool val)
	{ write<std::uint8_t>(val); }

	std::string readString();
	void writeString(const std::string& str);

private:
	void expand(std::size_t size);

	std::vector<char> m_data;
	std::size_t m_cursor;
};
