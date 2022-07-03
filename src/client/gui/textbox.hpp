#pragma once

#include <utility>

#include "common.hpp"
#include "widget.hpp"
#include "image.hpp"

#include "locale/unicodestring.hpp"

mGUI_BEGIN

class TextBox : public Widget
{
public:
	using EnterClickCallback = std::function<void()>;
	using CharVerifyCallback = std::function<bool(Unicode::Codepoint)>;

	enum Align
	{
		AlignLeft,
		AlignRight,
		AlignCenter
	};

	void update() override;
	void render() override;
	void onKey(Key key, KeyState state) override;
	void onKeyChar(KeyChar chr) override;

	inline void setOnEnter(const EnterClickCallback& enterClickCallback) { m_enterClickCallback = enterClickCallback; }
	inline void setCharValidator(const CharVerifyCallback& charValidator) { m_charValidator = charValidator; }

	constexpr void setTexture(Texture* texture) { m_texture = texture; }
	constexpr void setHoverTexture(Texture* texture) { m_hoverTexture = texture; }

	void put(Unicode::Codepoint cp);
	void putIfValid(Unicode::Codepoint chr);

	void setText(const U8String& text);
	[[nodiscard]] constexpr const U8String& getText() { return m_text; }

	void clear();

	void setCaretPos(u32 caretPos);
	[[nodiscard]] constexpr u32 getCaretPos() const { return m_caretPos; }

	constexpr void setFont(const std::string& font) { m_font = font; }
	[[nodiscard]] constexpr const std::string& getFont() const { return m_font; }

	constexpr void setFontScale(float fontScale) { m_fontScale = fontScale; }
	[[nodiscard]] constexpr float getFontScale() const { return m_fontScale; }

	constexpr void setTextSidePadding(float sidePadding) { m_textSidePadding = sidePadding; }
	[[nodiscard]] constexpr float getTextSidePadding() const { return m_textSidePadding; }

	constexpr void setTextYOffset(float yOffset) { m_textYOffset = yOffset; }
	[[nodiscard]] constexpr float getTextYOffset() const { return m_textYOffset; }

	constexpr void setMaxCharacters(u32 maxCharacters) { m_maxTextSize = maxCharacters; }
	[[nodiscard]] constexpr u32 getMaxCharacters() const { return m_maxTextSize; }

	constexpr void setReadOnly(bool readOnly) { m_readOnly = readOnly; }
	[[nodiscard]] constexpr bool getReadOnly() const { return m_readOnly; }

	inline void setFontColor(PixelRGBA8 color) { m_fontColor = std::move(color); }
	[[nodiscard]] inline PixelRGBA8 getFontColor() const { return m_fontColor; }

	constexpr void setCaretFixed(bool fixedCaret) { m_fixedCaret = fixedCaret; }
	[[nodiscard]] constexpr bool getCaretFixed() const { return m_fixedCaret; }

	constexpr void setCaretHidden(bool caretHidden) { m_hideCaret = caretHidden; }
	[[nodiscard]] constexpr bool getCaretHidden() const { return m_hideCaret; }

	constexpr void setBackspaceDisabled(bool disableBackspace) { m_disableBackspace = disableBackspace; }
	[[nodiscard]] constexpr bool getBackspaceDisabled() const { return m_disableBackspace; }

	constexpr void setClipboardDisabled(bool disableClipboard) { m_disableClipboard = disableClipboard; }
	[[nodiscard]] constexpr bool getClipboardDisabled() const { return m_disableClipboard; }

	constexpr void setTextAlign(Align textAlign) { m_textAlign = textAlign; }
	[[nodiscard]] constexpr Align getTextAlign() const { return m_textAlign; }

private:
	EnterClickCallback m_enterClickCallback;
	CharVerifyCallback m_charValidator;
	Texture* m_texture = nullptr;
	Texture* m_hoverTexture = nullptr;
	mGUI::Image m_image;
	Texture m_textTexture;
	U8String m_text;
	u32 m_caretPos = 0;
	float m_textSidePadding = 1.0f;
	float m_textYOffset = 1.0f;
	std::string m_font = "arial";
	float m_fontScale = 1.0f;
	u32 m_maxTextSize = UINT32_MAX;
	bool m_readOnly = false;
	PixelRGBA8 m_fontColor = Colors<Pixel::RGBA8>::Black;
	bool m_fixedCaret = false;
	bool m_hideCaret = false;
	bool m_disableBackspace = false;
	bool m_disableClipboard = false;
	Align m_textAlign = AlignLeft;
	bool m_beingHovered = false;
	float m_caretTimer = 0.0f;
	bool m_caretVisible = true;
};

mGUI_END
