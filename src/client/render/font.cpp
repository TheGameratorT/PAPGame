#include "font.hpp"

namespace Font {

i32 getMaxGlyphHeight(const TrueType::Font& font)
{
	const TrueType::FontHeader& fontHeader = font.header;
	i32 fontHeight = fontHeader.yMax - fontHeader.yMin;
	return fontHeight;
}

}
