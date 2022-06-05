#pragma once

#include <functional>

#include "common.hpp"
#include "widget.hpp"
#include "render/texture.hpp"

mGUI_BEGIN

class Image : public Widget
{
public:
	void render() override;

	constexpr void setTexture(Texture* texture) { m_texture = texture; }
	constexpr void setOpacity(float opacity) { m_textureOpacity = opacity; }
	constexpr void setRotation(const Vec3f& rotation) { m_rotation = rotation; }

private:
	Texture* m_texture = nullptr;
	float m_textureOpacity = 1.0f;
	Vec3f m_rotation = {};
};

mGUI_END
