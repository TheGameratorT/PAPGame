#pragma once

#include <functional>

#include "common.hpp"
#include "widget.hpp"
#include "render/texture.hpp"

mGUI_BEGIN

class Image : public Widget
{
public:
	using MouseClickCallback = std::function<void()>;

	void update() override;
	void render() override;
	void onClick() override;

	constexpr void setTexture(Texture& texture)
	{ m_texture = &texture; }

	constexpr void setHoverTexture(Texture& texture)
	{ m_hoverTexture = &texture; }

	constexpr void setHeldTexture(Texture& texture)
	{ m_heldTexture = &texture; }

	inline void setOnClick(const MouseClickCallback& mouseClickCallback)
	{ m_mouseClickCallback = mouseClickCallback; }

	constexpr void setOpacity(float opacity)
	{ m_textureOpacity = opacity; }

	constexpr void setRotation(const Vec3f& rotation)
	{ m_rotation = rotation; }

private:
	Texture* m_texture = nullptr;
	Texture* m_hoverTexture = nullptr;
	Texture* m_heldTexture = nullptr;
	float m_textureOpacity = 1.0f;
	MouseClickCallback m_mouseClickCallback;
	Vec3f m_rotation = {};
};

mGUI_END
