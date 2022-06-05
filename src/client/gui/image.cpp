#include "image.hpp"

#include "render/renderer.hpp"

mGUI_BEGIN

void Image::render()
{
	if (m_texture)
		Renderer::renderTexture(*m_texture, getBounds(), m_rotation, m_textureOpacity);
}

mGUI_END
