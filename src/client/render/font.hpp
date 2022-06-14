#pragma once

// Originally by Ed_IT

#include "math/vector.hpp"
#include "render/gle/texture2d.hpp"
#include "locale/unicodestring.hpp"
#include "font/truetype/truetype.hpp"
#include "font/rasterizer.hpp"

template<class String>
struct FontString
{
	String string;
	Vec2i canvasSize;
	i32 glyphYMax, glyphYMin;
	float fontScale;
	i32 caretX;
};

namespace Font {

template<class String>
FontString<String> calculateFontString(const TrueType::Font& msgFont, const String& string, float fontScale = 1.0f, u32 caretIndex = 0)
requires(TT::IsAnyOf<String, std::string, std::wstring, U8String, U16String, U32String>)
{
	Vec2i size;
	Vec2i caret;
	i32 glyphYMin = INT32_MAX;
	i32 glyphYMax = INT32_MIN;
	i32 caretX = 0.0f;

	for (SizeT i = 0; i < string.size(); i++) {

		Unicode::Codepoint cp = string[i];

		if (cp == '\n') {
			size.x = Math::max(size.x, caret.x);

			caret.x = 0;
			caret.y++;
			continue;
		}

		u32 glyphIndex = 0;

		if (msgFont.charMap.contains(cp)) {
			glyphIndex = msgFont.charMap.at(cp);
		}

		auto& glyph = msgFont.glyphs[glyphIndex];

		caret.x += Math::round(glyph.advance * fontScale);
		if (i < caretIndex)
			caretX = caret.x;
		glyphYMin = Math::min(glyphYMin, glyph.yMin);
		glyphYMax = Math::max(glyphYMax, glyph.yMax);

	}

	i32 glyphHeight = glyphYMax - glyphYMin;

	size.x = Math::max(size.x, caret.x);
	size.y = (caret.y + 1) * Math::round(glyphHeight * fontScale);

	return { string, size, glyphYMax, glyphYMin, fontScale, caretX };
}

template<class String>
void renderFontString(const TrueType::Font& msgFont, GLE::Texture2D& texture, const FontString<String>& string, const Vec2i& position = {}, const Vec2i& border = {}, PixelType<Pixel::RGBA8> color = Colors<Pixel::RGBA8>::Black)
requires(TT::IsAnyOf<String, std::string, std::wstring, U8String, U16String, U32String>)
{
	Image<Pixel::RGBA8> canvas(string.canvasSize.x + border.x, string.canvasSize.y + border.y);
	canvas.clear({0, 0, 0, 0});

	i32 glyphHeight = string.glyphYMax - string.glyphYMin;

	Vec2i caret = position;
	caret.y += string.canvasSize.y - Math::round(string.glyphYMax * string.fontScale);

	// Create canvas and render string
	for (SizeT i = 0; i < string.string.size(); i++) {

		Unicode::Codepoint cp = string.string[i];

		if (cp == '\n') {
			caret.x = 0;
			caret.y -= Math::round(glyphHeight * string.fontScale);
			continue;
		}

		u32 glyphIndex = 0;

		if (msgFont.charMap.contains(cp)) {
			glyphIndex = msgFont.charMap.at(cp);
		}

		auto& glyph = msgFont.glyphs[glyphIndex];

		Font::rasterize(msgFont, canvas, caret, glyph, string.fontScale, color);

		caret.x += Math::round(glyph.advance * string.fontScale);

	}

	canvas.flipY();
	auto pixels = canvas.getImageBuffer();

	texture.create();
	texture.bind();
	texture.setData(canvas.getWidth(), canvas.getHeight(), GLE::ImageFormat::RGBA8, GLE::TextureSourceFormat::RGBA, GLE::TextureSourceType::UByte, pixels.data());
	texture.setMinFilter(GLE::TextureFilter::Trilinear, true);
	texture.setMagFilter(GLE::TextureFilter::Bilinear);
	texture.generateMipmaps();
}

i32 getMaxGlyphHeight(const TrueType::Font& font);

}
