#include "texture.hpp"

#include "render/gle/gle.hpp"
#include "image/image.hpp"
#include "arc+/image/imageio.hpp"

void Texture::load(const Path& path, GLE::TextureFilter magFilter)
{
	Image image = ImageIO::loadE<Pixel::RGBA8>(path);

	m_texture2D.create();
	m_texture2D.bind();

	m_texture2D.setData(
		image.getWidth(),
		image.getHeight(),
		GLE::ImageFormat::RGBA8,
		GLE::TextureSourceFormat::RGBA,
		GLE::TextureSourceType::UByte,
		image.getImageBuffer().data()
	);

	m_texture2D.setWrapU(GLE::TextureWrap::Repeat);
	m_texture2D.setWrapV(GLE::TextureWrap::Repeat);
	m_texture2D.setMinFilter(GLE::TextureFilter::Trilinear, true);
	m_texture2D.setMagFilter(magFilter);

	m_texture2D.generateMipmaps();
}

void Texture::destroy()
{
	m_texture2D.destroy();
}
