#pragma once

// TODO: Support for UTF16 and UTF32 endianness conversion (low priority)

#include "types.hpp"
#include "common.hpp"
#include "common/concepts.hpp"
#include "locale/unicodestring.hpp"

#include "util/bits.hpp"

#include <string>
#include <vector>

NETWORK_BEGIN

class PacketBuffer
{
public:
	PacketBuffer();
	explicit PacketBuffer(std::vector<char> data);

	void reserve(SizeT size);
	void seek(SizeT size);
	[[nodiscard]] SizeT readableBytes() const;

	constexpr
	std::vector<char>& getData()
	{ return m_data; }

	[[nodiscard]] constexpr
	const std::vector<char>& getData() const
	{ return m_data; }

	template<CC::Integral T>
	T read()
	{
		T val = readLE<T>(&m_data[m_cursor]);
		m_cursor += sizeof(T);
		return val;
	}

	template<CC::Integral T>
	void write(T val)
	{
		SizeT writeSize = sizeof(T);
		expand(writeSize);
		writeLE(&m_data[m_cursor], val);
		m_cursor += writeSize;
	}

	inline bool readBool()
	{ return read<u8>(); }

	inline void writeBool(bool val)
	{ write<u8>(val); }

	std::string readString();
	void writeString(const std::string& str);

	template<Unicode::Encoding T>
	UnicodeString<T> readUtfString()
	{
		char* cur = &m_data[m_cursor];
		SizeT strSize = readLE<u32>(cur);
		cur += sizeof(u32);
		UnicodeString<T> out;
		out.resizeBytes(strSize);
		std::memcpy(out.data(), cur, strSize);
		m_cursor += sizeof(u32) + strSize;
		return out;
	}

	template<Unicode::Encoding T>
	void writeUtfString(const UnicodeString<T>& str)
	{
		SizeT strSize = str.rawSize();
		SizeT writeSize = sizeof(u32) + strSize;
		expand(writeSize);
		writeLE(&m_data[m_cursor], u32(strSize));
		const auto* strData = str.data();
		std::memcpy(&m_data[m_cursor + sizeof(u32)], strData, strSize);
		m_cursor += writeSize;
	}

	inline U8String readU8String() { return readUtfString<Unicode::UTF8>(); }
	inline U16String readU16String() { return readUtfString<Unicode::UTF16>(); }
	inline U32String readU32String() { return readUtfString<Unicode::UTF32>(); }

	inline void writeU8String(const U8String& str) { writeUtfString(str); }
	inline void writeU16String(const U16String& str) { writeUtfString(str); }
	inline void writeU32String(const U32String& str) { writeUtfString(str); }

	template<CC::Arithmetic T>
	static inline T readLE(char* buf)
	{
		T t;
		std::memcpy(&t, buf, sizeof(T));
		return Bits::little(t);
	}

	template<CC::Arithmetic T>
	static inline void writeLE(char* buf, T val)
	{
		T sval = Bits::little(val);
		std::memcpy(buf, &sval, sizeof(T));
	}

private:
	void expand(SizeT size);

	std::vector<char> m_data;
	SizeT m_cursor;
};

NETWORK_END
