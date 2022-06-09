#pragma once

#include "common.hpp"
#include "widget.hpp"
#include "image.hpp"

#include "locale/unicodestring.hpp"

mGUI_BEGIN

class TextBox : public Widget
{
public:
	void update() override;
	void render() override;
	void onKey(Key key, KeyState state) override;
	void onKeyChar(KeyChar chr) override;

	constexpr void setTexture(Texture* texture) { m_image.setTexture(texture); }

	void setText(const U8String& text);
	[[nodiscard]] constexpr const U8String& getText() { return m_text; }

	constexpr void setFontScale(float fontScale) { m_fontScale = fontScale; }
	[[nodiscard]] constexpr float getFontScale() const { return m_fontScale; }

	constexpr void setTextSidePadding(float sidePadding) { m_textSidePadding = sidePadding; }
	[[nodiscard]] constexpr float getTextSidePadding() const { return m_textSidePadding; }

	constexpr void setMaxCharacters(u32 maxCharacters) { m_maxTextSize = maxCharacters; }
	[[nodiscard]] constexpr u32 getMaxCharacters() const { return m_maxTextSize; }

private:
	mGUI::Image m_image;
	mGUI::Image m_textImage;
	Texture m_textTexture;
	U8String m_text;
	float m_textSidePadding = 1.0f;
	float m_fontScale = 1.0f;
	u32 m_maxTextSize = UINT32_MAX;
};

mGUI_END