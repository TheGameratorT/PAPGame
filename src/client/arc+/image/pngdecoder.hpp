#pragma once

#include "image/image.hpp"
#include "image/decode/decoder.hpp"

class PNGDecoder : public IImageDecoder
{
public:
	explicit PNGDecoder(std::optional<Pixel> reqFormat) : IImageDecoder(reqFormat), validDecode(false) {}

	void decode(std::span<const u8> data);
	RawImage& getImage();

private:
	bool validDecode;
	RawImage image;
};
