#pragma once

// Arclight Image Extension

#include "image/image.hpp"
#include "image/imageio.hpp"
#include "image/decode/bitmapdecoder.hpp"

#include "pngdecoder.hpp"

namespace ImageIO
{
	template<Pixel P>
	Image<P> loadE(const Path& path)
	{
		std::string ext = path.getExtension();

		if (ext == ".png") {
			return load<P, PNGDecoder>(path);
		}

		return ImageIO::load<P>(path);
	}
}
