#pragma once

#include "render/gle/texture2d.hpp"
#include "filesystem/path.hpp"

class Texture
{
public:
	void load(const Path& path);
	void destroy();

	[[nodiscard]] constexpr GLE::Texture2D& getGleTex2D()
	{ return m_texture2D; }

private:
	GLE::Texture2D m_texture2D{};
};
