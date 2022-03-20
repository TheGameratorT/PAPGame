#pragma once

#include <cstdint>
#include <memory>

class Image
{
public:
	enum class ColorFormat
	{
		RGB = 0,
		RGBA = 1,
		BGR = 2
	};

	static Image loadPng(const char* name);

	static Image loadBmp(const char* name);

	[[nodiscard]] constexpr std::uint32_t getWidth() const
	{ return m_width; }

	[[nodiscard]] constexpr std::uint32_t getHeight() const
	{ return m_height; }

	[[nodiscard]] constexpr ColorFormat getColorFormat() const
	{ return m_colorFormat; }

	[[nodiscard]] const char* getData() const
	{ return m_data.get(); }

private:
	std::uint32_t m_width;
	std::uint32_t m_height;
	ColorFormat m_colorFormat;
	std::unique_ptr<char[]> m_data;
};
