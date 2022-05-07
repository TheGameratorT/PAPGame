#pragma once

#include "render/gle/texture2d.hpp"
#include "math/matrix.hpp"

#include "texture.hpp"

namespace Renderer
{
	void init();
	void destroy();

	void renderTexture(Texture& texture, const Mat4f& matrix);
}
