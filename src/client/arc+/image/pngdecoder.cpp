#include "pngdecoder.hpp"

#include <png.h>

#include "stream/binaryreader.hpp"

void PNGDecoder::decode(std::span<const u8> data)
{
	png_uint_32 retval;

	BinaryReader reader(data, ByteOrder::Little);

	u8 sigData[8];
	reader.read<u8>({sigData, 8});
	if (png_sig_cmp(sigData, 0, 8) != 0)
		throw ImageDecoderException("Invalid PNG signature");

	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);
	if (png_ptr == nullptr)
		throw ImageDecoderException("Failed to create PNG read structure");

	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == nullptr)
	{
		png_destroy_read_struct(&png_ptr, nullptr, nullptr);
		throw ImageDecoderException("Failed to create PNG info structure");
	}

	png_set_read_fn(png_ptr, &reader, [](png_structp png_ptr, png_bytep outBytes, png_size_t byteCountToRead)
	{
		BinaryReader& reader = *reinterpret_cast<BinaryReader*>(png_get_io_ptr(png_ptr));

		if (byteCountToRead > reader.remainingSize())
			throw ImageDecoderException("Invalid PNG read, tried to read past end of file");

		reader.read(std::span<u8>(outBytes, byteCountToRead));
	});

	png_set_sig_bytes(png_ptr, 8);

	png_read_info(png_ptr, info_ptr);

	png_uint_32 width = 0;
	png_uint_32 height = 0;
	int bitDepth = 0;
	int colorType = -1;
	retval = png_get_IHDR(
		png_ptr, info_ptr,
		&width, &height,
		&bitDepth, &colorType,
		nullptr, nullptr, nullptr
	);

	if (retval != 1)
		throw ImageDecoderException("Failed to read PNG header");

	auto loadData = [&]<Pixel P>()
	{
		Image<P> newImage(width, height);
		using PixelT = PixelType<P>;
		constexpr int N = P == Pixel::RGBA8 ? 4 : 3;

		const png_uint_32 bytesPerRow = png_get_rowbytes(png_ptr, info_ptr);
		u8* rowData = new u8[bytesPerRow];

		for(u32 rowIdx = 0; rowIdx < height; ++rowIdx)
		{
			png_read_row(png_ptr, rowData, nullptr);

			u32 byteIndex = 0;
			for(u32 colIdx = 0; colIdx < width; ++colIdx)
			{
				newImage.setPixel(colIdx, rowIdx, PixelT({&rowData[byteIndex], N}));
				byteIndex += N;
			}
			arc_assert(byteIndex == bytesPerRow, "Byte index did not match bytes per row");
		}

		delete[] rowData;

		image = newImage.makeRaw();
	};

	switch(colorType)
	{
	case PNG_COLOR_TYPE_RGB:
		loadData.template operator()<Pixel::RGB8>();
		break;
	case PNG_COLOR_TYPE_RGB_ALPHA:
		loadData.template operator()<Pixel::RGBA8>();
		break;
	default:
		png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
		throw ImageDecoderException("Unsupported PNG ColorType returned");
	}

	png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);

	validDecode = true;
}

RawImage& PNGDecoder::getImage()
{
	if (!validDecode)
		throw ImageDecoderException("Bad image decode");

	return image;
}
