#include "image.hpp"

#include "render/renderer.hpp"

mGUI_BEGIN

void Image::update()
{
}

void Image::render()
{
	if (m_texture != nullptr)
		Renderer::renderTexture(*m_texture, bounds, m_rotation, m_textureOpacity);
}

bool Image::onClick(Key key, KeyState state)
{
	if (m_mouseClickCallback)
		return m_mouseClickCallback(key, state);
	return true;
}

mGUI_END
