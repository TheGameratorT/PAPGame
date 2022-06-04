#include "image.hpp"

#include "render/renderer.hpp"

mGUI_BEGIN

void Image::update()
{
}

void Image::render()
{
	if (m_texture)
	{
		Texture* texture;
		if (m_heldTexture && beingHeld)
			texture = m_heldTexture;
		else if (m_hoverTexture && beingHovered && hoverable)
			texture = m_hoverTexture;
		else
			texture = m_texture;
		Renderer::renderTexture(*texture, bounds, m_rotation, m_textureOpacity);
	}
}

void Image::onClick()
{
	if (m_mouseClickCallback)
		return m_mouseClickCallback();
}

mGUI_END
