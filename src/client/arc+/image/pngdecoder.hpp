#pragma once

#include "image/image.hpp"
#include "image/decode/decoder.hpp"

class PNGDecoder : public IImageDecoder
{
public:
	PNGDecoder() : validDecode(false) {}

	void decode(std::span<const u8> data);

	template<Pixel P>
	Image<P> getImage()
	{
		if (!validDecode) {
			throw ImageDecoderException("Bad image decode");
		}
		return Image<P>::fromRaw(image);
	}

private:
	bool validDecode;
	RawImage image;
};
