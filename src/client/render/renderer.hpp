#pragma once

#include "render/gle/texture2d.hpp"
#include "math/matrix.hpp"
#include "math/rectangle.hpp"

#include "texture.hpp"

namespace Renderer
{
	void init();
	void destroy();

	void updateProjection();

	void renderTexture(Texture& texture, const Vec3i& position, const Vec3f& rotation, const Vec3f& scale, float opacity);
	void renderTexture(Texture& texture, const RectI& rectangle, const Vec3f& rotation, float opacity);
}
