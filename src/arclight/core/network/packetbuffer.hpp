#pragma once

#include "types.hpp"
#include "common.hpp"
#include "ipacketserializeable.hpp"
#include "common/concepts.hpp"
#include "common/typetraits.hpp"
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

	template<class T> requires CC::BaseOf<IPacketSerializeable, T>
	inline T read()
	{
		T val;
		val.packetRead(*this);
		return val;
	}

	template<class T> requires CC::BaseOf<IPacketSerializeable, T>
	inline void write(const T& val)
	{
		val.packetWrite(*this);
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
		SizeT strSize = read<u32>();
		UnicodeString<T> out;
		if (strSize != 0)
		{
			if constexpr (Unicode::isUTF8<T>())
			{
				std::basic_string<char8_t> strData(strSize, 0);
				std::memcpy(strData.data(), &m_data[m_cursor], strSize);
				out.assign(strData);
				m_cursor += strSize;
			}
			else
			{
				using U = TT::Conditional<Unicode::isUTF16<T>(), char16_t, char32_t>;
				SizeT strDataSize = strSize / sizeof(U);
				std::basic_string<U> strData(strDataSize, 0);
				for (SizeT i = 0; i < strDataSize; i++)
					strData[i] = read<U>();
				out.assign(strData);
			}
		}
		return out;
	}

	template<Unicode::Encoding T>
	void writeUtfString(const UnicodeString<T>& str)
	{
		SizeT strSize = str.rawSize();
		SizeT writeSize = sizeof(u32) + strSize;
		reserve(writeSize); // pre-allocate

		write(u32(strSize));

		if (strSize != 0)
		{
			expand(strSize);

			const auto* strData = str.data();
			if constexpr (Unicode::isUTF8<T>())
			{
				std::memcpy(&m_data[m_cursor], strData, strSize);
			}
			else
			{
				using U = TT::Conditional<Unicode::isUTF16<T>(), char16_t, char32_t>;
				SizeT strDataLESize = strSize / sizeof(U);
				std::basic_string<U> strDataLE(strDataLESize, 0);
				std::memcpy(strDataLE.data(), strData, strSize);
				for (SizeT i = 0; i < strDataLESize; i++)
					strDataLE[i] = Bits::little(strDataLE[i]);
				std::memcpy(&m_data[m_cursor], strDataLE.data(), strSize);
			}
			m_cursor += strSize;
		}
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
