#include "textbox.hpp"

#include "font/truetype/truetype.hpp"
#include "font/rasterizer.hpp"
#include "render/font.hpp"
#include "render/renderer.hpp"

#include "game.hpp"

mGUI_BEGIN

void TextBox::update()
{
	//Log::info("TEXT", (const char*)m_text.cStr());
	//ArcDebug() << m_text;

	if (!m_readOnly)
	{
		Vec2d cursorPos = Game::getCursorPosition();
		bool isContained = getBounds().contains(cursorPos);
		if (m_beingHovered && !isContained)
		{
			m_beingHovered = false;
		}
		else if (isContained)
		{
			m_beingHovered = true;
		}

		m_caretTimer += Game::getTickDuration();
		if (m_caretTimer > 0.5f)
		{
			m_caretVisible = !m_caretVisible;
			m_caretTimer = 0.0f;
		}
	}
	else
	{
		m_beingHovered = false;
		m_caretVisible = false;
	}

	Texture* texture;
	if (m_hoverTexture && (m_beingHovered || Game::getGUI().isWidgetFocused(this)))
		texture = m_hoverTexture;
	else
		texture = m_texture;
	m_image.setTexture(texture);
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

	auto fontStr = Font::calculateFontString(font, m_text, fontScale * float(qualityMul), m_caretPos);

	i32 xOffset = std::lroundl(m_textSidePadding * fontScale);
	i32 yOffset = std::lroundl((float(fontHeight) + m_textYOffset) * fontScale);

	if (m_textAlign == AlignCenter)
	{
		xOffset += std::lroundl(float(bounds.getWidth() - fontStr.canvasSize.x) / 2.0f);
	}
	else if (m_textAlign == AlignRight)
	{
		xOffset += bounds.getWidth() - fontStr.canvasSize.x;
	}

	i32 fontX = bounds.x + xOffset;

	if (!m_text.empty())
	{
		i32 fontY = bounds.y + yOffset + std::lroundl(-float(fontStr.glyphYMax) * fontScale);

		Font::renderFontString(font, m_textTexture.getGleTex2D(), fontStr, {}, {}, m_fontColor);
		RectI textBounds = {
			fontX,
			fontY,
			i32(m_textTexture.getWidth()) / qualityMul,
			i32(m_textTexture.getHeight()) / qualityMul
		};
		Renderer::renderTexture(m_textTexture, textBounds, {}, 1.0f);
		m_textTexture.destroy();
	}

	if (!m_hideCaret && m_caretVisible && Game::getGUI().isWidgetFocused(this))
	{
		i32 caretSize = std::lroundl(1860.0f * fontScale);
		Texture& caretTex = Game::getGUI().getCaretTexture();
		RectI caretBounds = {
			fontX + fontStr.caretX - i32(std::lroundl(192.0f * fontScale)),
			bounds.y + yOffset - i32(std::lroundl(1600.0f * fontScale)),
			caretSize,
			caretSize
		};
		Renderer::renderTexture(caretTex, caretBounds, {}, 1.0f);
	}
}

void TextBox::onKey(Key key, KeyState state)
{
	if (m_readOnly)
		return;

	if (key >= MouseCode::Button1 && key <= MouseCode::Button8)
	{
		// Mouse Press
		if (key == MouseCode::Left && state == KeyState::Pressed)
		{
			Vec2d cursorPos = Game::getCursorPosition();
			bool isContained = getBounds().contains(cursorPos);
			if (isContained)
			{
				Game::getGUI().setFocusedWidget(this);
			}
		}
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

	if (Game::getGUI().isWidgetFocused(this))
	{
		if (state == KeyState::Pressed || state == KeyState::Held)
		{
			if (key == KeyCode::Backspace)
			{
				if (!m_disableBackspace && m_caretPos != 0)
				{
					m_text.erase(m_caretPos - 1, 1);
					m_caretPos--;
				}
			}
			else if (key == KeyCode::Left)
			{
				if (!m_fixedCaret && m_caretPos != 0)
					m_caretPos--;
			}
			else if (key == KeyCode::Right)
			{
				if (!m_fixedCaret && m_caretPos != m_text.size())
					m_caretPos++;
			}
		}

		if (!m_disableClipboard && Game::getGUI().getCtrlDown() && state == KeyState::Pressed && key == KeyCode::V)
		{
			U8String str = Game::getClipboard();
			SizeT strSize = str.size();
			for (SizeT i = 0; i < strSize; i++)
			{
				if (m_text.size() >= m_maxTextSize)
					break;
				putIfValid(str[i]);
			}
		}
	}
}

void TextBox::onKeyChar(KeyChar chr)
{
	if (m_readOnly || !Game::getGUI().isWidgetFocused(this) || Game::getGUI().getCtrlDown() || m_text.size() >= m_maxTextSize)
		return;

	putIfValid(Unicode::Codepoint(chr));
}

void TextBox::put(Unicode::Codepoint cp)
{
	m_text.insert(m_caretPos, cp);
	m_caretPos++;
}

void TextBox::putIfValid(Unicode::Codepoint cp)
{
	bool allow = true;
	if (m_charValidator)
		allow = m_charValidator(cp);
	if (!allow)
		return;

	put(cp);
}

void TextBox::setText(const U8String& text)
{
	m_text = text;
	m_caretPos = 0;
}

void TextBox::clear()
{
	m_text.clear();
	m_caretPos = 0;
}

void TextBox::setCaretPos(u32 caretPos)
{
	u32 textSize = m_text.size();
	if (caretPos > textSize)
		caretPos = textSize;
	m_caretPos = caretPos;
}

mGUI_END
