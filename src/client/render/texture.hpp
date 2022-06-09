#pragma once

#include "render/gle/texture2d.hpp"
#include "filesystem/path.hpp"
#include "image/image.hpp"

class Texture
{
public:
	void load(const Path& path, GLE::TextureFilter magFilter = GLE::TextureFilter::Bilinear);
	void fromImage(Image<Pixel::RGBA8>& image, GLE::TextureFilter magFilter = GLE::TextureFilter::Bilinear);
	void destroy();

	[[nodiscard]] constexpr GLE::Texture2D& getGleTex2D()
	{ return m_texture2D; }

	[[nodiscard]] inline u32 getWidth() const
	{ return m_texture2D.getWidth(); }

	[[nodiscard]] inline u32 getHeight() const
	{ return m_texture2D.getHeight(); }

private:
	GLE::Texture2D m_texture2D{};
};
