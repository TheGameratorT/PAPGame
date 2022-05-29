#pragma once

#include <functional>

#include "common.hpp"
#include "iwidget.hpp"
#include "render/texture.hpp"

mGUI_BEGIN

class Image : public IWidget
{
public:
	using MouseClickCallback = std::function<bool(Key key, KeyState state)>;

	void update() override;
	void render() override;
	bool onClick(Key key, KeyState state) override;

	constexpr void setTexture(Texture& texture)
	{ m_texture = &texture; }

	inline void setOnClick(const MouseClickCallback& mouseClickCallback)
	{ m_mouseClickCallback = mouseClickCallback; }

	constexpr void setOpacity(float opacity)
	{ m_textureOpacity = opacity; }

	constexpr void setRotation(const Vec3f& rotation)
	{ m_rotation = rotation; }

private:
	Texture* m_texture = nullptr;
	float m_textureOpacity = 1.0f;
	MouseClickCallback m_mouseClickCallback;
	Vec3f m_rotation = {};
};

mGUI_END
