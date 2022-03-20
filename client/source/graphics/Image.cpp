#include "Image.hpp"

#include <fstream>

#include <png.h>

Image Image::loadPng(const char* name)
{
	FILE* fp;
	if (fopen_s(&fp, name, "rb") != 0)
		throw std::runtime_error(std::string("[IMAGE-PNG] Failed to open PNG file: ") + name);

	png_byte header[8];
	fread(header, 1, 8, fp);
	if (png_sig_cmp(header, 0, 8) != 0)
	{
		fclose(fp);
		throw std::runtime_error(std::string("[IMAGE-PNG] Invalid PNG signature for file: ") + name);
	}

	/* Create and initialize the png_struct
	 * with the desired error handler
	 * functions.  If you want to use the
	 * default stderr and longjump method,
	 * you can supply NULL for the last
	 * three parameters.  We also supply the
	 * the compiler header file version, so
	 * that we know if the application
	 * was compiled with a compatible version
	 * of the library.  REQUIRED
	 */
	png_structp png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, nullptr, nullptr, nullptr);

	if (png_ptr == nullptr)
	{
		fclose(fp);
		throw std::runtime_error(std::string("[IMAGE-PNG] Failed to create PNG read structure for file: ") + name);
	}

	/* Allocate/initialize the memory
	 * for image information.  REQUIRED. */
	png_infop info_ptr = png_create_info_struct(png_ptr);
	if (info_ptr == nullptr)
	{
		fclose(fp);
		png_destroy_read_struct(&png_ptr, nullptr, nullptr);
		throw std::runtime_error(std::string("[IMAGE-PNG] Failed to create PNG info structure for file: ") + name);
	}

	/* Set error handling if you are
	 * using the setjmp/longjmp method
	 * (this is the normal method of
	 * doing things with libpng).
	 * REQUIRED unless you  set up
	 * your own error handlers in
	 * the png_create_read_struct()
	 * earlier.
	 */
	if (setjmp(png_jmpbuf(png_ptr))) {
		/* Free all of the memory associated
		 * with the png_ptr and info_ptr */
		png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
		fclose(fp);
		/* If we get here, we had a
		 * problem reading the file */
		throw std::runtime_error(std::string("[IMAGE-PNG] Failed to do something related to setjmp in PNG: ") + name);
	}

	/* Set up the output control if
	 * you are using standard C streams */
	png_init_io(png_ptr, fp);

	/* If we have already
	 * read some of the signature */
	png_set_sig_bytes(png_ptr, 8);

	/*
	 * If you have enough memory to read
	 * in the entire image at once, and
	 * you need to specify only
	 * transforms that can be controlled
	 * with one of the PNG_TRANSFORM_*
	 * bits (this presently excludes
	 * dithering, filling, setting
	 * background, and doing gamma
	 * adjustment), then you can read the
	 * entire image (including pixels)
	 * into the info structure with this
	 * call
	 *
	 * PNG_TRANSFORM_STRIP_16 |
	 * PNG_TRANSFORM_PACKING  forces 8 bit
	 * PNG_TRANSFORM_EXPAND forces to
	 *  expand a palette into RGB
	 */
	png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_STRIP_16 | PNG_TRANSFORM_PACKING | PNG_TRANSFORM_EXPAND, nullptr);

	Image image;

	int bit_depth, color_type, interlace_type;
	png_get_IHDR(png_ptr, info_ptr, &image.m_width, &image.m_height, &bit_depth, &color_type, &interlace_type, nullptr, nullptr);

	switch(color_type)
	{
	case PNG_COLOR_TYPE_RGB:
		image.m_colorFormat = ColorFormat::RGB;
		break;
	case PNG_COLOR_TYPE_RGBA:
		image.m_colorFormat = ColorFormat::RGBA;
		break;
	default:
		png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);
		fclose(fp);
		throw std::runtime_error(std::string("[IMAGE-PNG] Unsupported color format for PNG file: ") + name);
	}

	unsigned int row_bytes = png_get_rowbytes(png_ptr, info_ptr);
	image.m_data = std::make_unique<char[]>(row_bytes * image.m_height);

	png_bytepp row_pointers = png_get_rows(png_ptr, info_ptr);

	for (int i = 0; i < image.m_height; i++) {
		// note that png is ordered top to
		// bottom, but OpenGL expect it bottom to top
		// so the order or swapped
		memcpy(&image.m_data[(row_bytes * (image.m_height-1-i))], row_pointers[i], row_bytes);
	}

	/* Clean up after the read,
	 * and free any memory allocated */
	png_destroy_read_struct(&png_ptr, &info_ptr, nullptr);

	/* Close the file */
	fclose(fp);

	/* That's it */
	return image;
}

Image Image::loadBmp(const char* name)
{
	// Data read from the header of the BMP file
	unsigned char header[54]; // Each BMP file begins by a 54-bytes header
	unsigned int dataPos;     // Position in the file where the actual data begins
	unsigned int width, height;
	unsigned int imageSize;   // = width*height*3

	// Open the file
	FILE* fp;
	if (fopen_s(&fp, name, "rb") != 0)
		throw std::runtime_error(std::string("[IMAGE-BMP] Failed to open BMP file: ") + name);

	if (fread(header, 1, 54, fp) != 54)
		throw std::runtime_error(std::string("[IMAGE-BMP] Failed to read header of BMP file: ") + name);

	if (header[0] != 'B' || header[1] != 'M')
		throw std::runtime_error(std::string("[IMAGE-BMP] Invalid header for BMP file: ") + name);

	// Read ints from the byte array
	dataPos    = *reinterpret_cast<int*>(&(header[0x0A]));
	imageSize  = *reinterpret_cast<int*>(&(header[0x22]));
	width      = *reinterpret_cast<int*>(&(header[0x12]));
	height     = *reinterpret_cast<int*>(&(header[0x16]));

	// Some BMP files are misformatted, guess missing information
	if (imageSize == 0) imageSize = width * height * 3; // 3 : one byte for each Red, Green and Blue component
	if (dataPos == 0)   dataPos = 54; // The BMP header is done that way

	// Create a buffer
	auto data = std::make_unique<char[]>(imageSize);

	// Read the actual data from the file into the buffer
	fread(data.get(), 1, imageSize, fp);

	//Everything is in memory now, the file can be closed
	fclose(fp);

	Image image;
	image.m_width = width;
	image.m_height = height;
	image.m_colorFormat = ColorFormat::BGR;
	image.m_data = std::move(data);

	return image;
}
