#include "textbox.hpp"

#include "font/truetype/truetype.hpp"
#include "font/rasterizer.hpp"
#include "render/font.hpp"

#include "game.hpp"

mGUI_BEGIN

void TextBox::update()
{
	//Log::info("TEXT", (const char*)m_text.cStr());
	//ArcDebug() << m_text;
}

void TextBox::render()
{
	const RectI& bounds = getBounds();

	m_image.setBounds(bounds);
	m_image.render();

	if (bounds.getHeight() == 0)
		return;

	const TrueType::Font& font = Game::getFont(m_font);

	i32 qualityMul = 1;

	i32 fontHeight = Font::getMaxGlyphHeight(font);
	float fontScale = (float(bounds.getHeight()) / float(fontHeight)) * m_fontScale;

	auto fontStr = Font::calculateFontString(font, m_text, fontScale * float(qualityMul));

	i32 xOffset = std::lroundl(m_textSidePadding * fontScale);

	i32 yOffset = std::lroundl(-float(fontStr.glyphYMax) * fontScale);
	yOffset += std::lroundl(float(fontHeight) * fontScale);

	Font::renderFontString(font, m_textTexture.getGleTex2D(), fontStr);
	m_textImage.setTexture(&m_textTexture);
	m_textImage.setBounds({
		bounds.x + xOffset,
		bounds.y + yOffset,
		i32(m_textTexture.getWidth()) / qualityMul,
		i32(m_textTexture.getHeight()) / qualityMul
	});
	m_textImage.render();
	m_textTexture.destroy();
}

void TextBox::onKey(Key key, KeyState state)
{
	if (key >= MouseCode::Button1 && key <= MouseCode::Button8)
	{
		// Mouse Press
		return;
	}

	if (state == KeyState::Pressed)
	{
		if (key == KeyCode::Enter)
		{
			if (m_enterClickCallback)
				m_enterClickCallback();
			return;
		}
	}

	if (state == KeyState::Pressed || state == KeyState::Held)
	{
		if (key == KeyCode::Backspace)
		{
			if (!m_text.empty())
				m_text.erase(m_text.size() - 1);
		}
	}

	/*if (Game::getGUI().getCtrlDown() && state == KeyState::Pressed && key == KeyCode::V)
	{
		std::string s = Game::getClipboard();
		SizeT newSize = m_text.size() + s.size();
		if (newSize > m_maxTextSize)
			newSize = m_maxTextSize;
		m_text.append(reinterpret_cast<const char32_t*>(s.c_str()), newSize);
	}*/
}

void TextBox::onKeyChar(KeyChar chr)
{
	if (Game::getGUI().getCtrlDown() || m_text.size() >= m_maxTextSize)
		return;

	bool allow = true;
	if (m_charValidator)
		allow = m_charValidator(chr);
	if (!allow)
		return;

	m_text += Unicode::Codepoint(chr);
}

void TextBox::setText(const U8String& text)
{
	m_text = text;
}

mGUI_END
