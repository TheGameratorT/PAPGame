#pragma once

#include <functional>

#include "common.hpp"
#include "widget.hpp"
#include "image.hpp"

#include "render/texture.hpp"

mGUI_BEGIN

class Button : public Widget
{
public:
	using MouseClickCallback = std::function<void()>;

	void update() override;
	void render() override;
	void onKey(Key key, KeyState state) override;

	inline void setOnClick(const MouseClickCallback& mouseClickCallback)
	{ m_mouseClickCallback = mouseClickCallback; }

	constexpr void setEnabled(bool enabled) { m_enabled = enabled; }
	[[nodiscard]] constexpr bool getEnabled() const { return m_enabled; }

	constexpr void setTexture(Texture* texture) { m_texture = texture; }
	constexpr void setHoverTexture(Texture* texture) { m_hoverTexture = texture; }
	constexpr void setHeldTexture(Texture* texture) { m_heldTexture = texture; }

private:
	MouseClickCallback m_mouseClickCallback;
	mGUI::Image m_image;
	Texture* m_texture = nullptr;
	Texture* m_hoverTexture = nullptr;
	Texture* m_heldTexture = nullptr;
	bool m_enabled = true;
	bool m_beingHovered = false;
	bool m_beingHeld = false;
};

mGUI_END
