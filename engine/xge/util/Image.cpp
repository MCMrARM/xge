#include "Image.h"

#include <png.h>
#include <xge/util/InputStream.h>
#include <xge/util/Resources.h>
#include <xge/util/DynamicStackArray.h>

using namespace xge;

unsigned int xge::GetBytesPerPixel(ImageFormat format) {
    switch (format) {
        case ImageFormat::GRAY:
            return 1;
        case ImageFormat::RGB:
            return 3;
        case ImageFormat::RGBA:
            return 4;
		default:
			return 0;
    }
}

void Image::copyTo(Image &img, int x, int y) {
    XGEAssert(img.format == format);
    XGEAssert(x + w > 0);
    XGEAssert(y + h > 0);
    XGEAssert(x + w <= img.w);
    XGEAssert(y + h <= img.h);
    unsigned int bytesPerPixel = GetBytesPerPixel(format);
    for (int i = 0; i < h; i++) {
        memcpy(&img.getData()[(img.getWidth() * (y + i) + x) * bytesPerPixel], &data[i * w * bytesPerPixel], w * bytesPerPixel);
    }
}

Image Image::fromStream(InputStream &stream) {
    char sig[8];
    XGEAssert(stream.read(sig, 8) == 8);
    int i = png_sig_cmp((png_bytep) sig, 0, 8);
    if (i != 0) {
#ifndef NDEBUG
        Log::warn("Image", "Not a valid png image: invalid signature");
#endif
        return Image();
    }

    png_struct *png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);
    if (!png_ptr) {
#ifndef NDEBUG
        Log::warn("Image", "Failed to allocate PNG read struct.");
#endif
        return Image();
    }

    png_info *info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
#ifndef NDEBUG
        Log::warn("Image", "Failed to allocate PNG info struct.");
#endif
        png_destroy_read_struct(&png_ptr, NULL, NULL);
        return Image();
    }

    png_set_read_fn(png_ptr, (void*) &stream, [](png_structp png_ptr, png_bytep out, png_size_t size) {
        InputStream *inputStream = (InputStream *) png_get_io_ptr(png_ptr);
        if (inputStream->read((char*) out, size) != size)
            png_error(png_ptr, "IO error");
    });
    png_set_sig_bytes(png_ptr, 8);
    png_read_info(png_ptr, info_ptr);

    ImageFormat format = ImageFormat::RGBA;

    int depth, colorType;
    png_uint_32 width, height;
    png_get_IHDR(png_ptr, info_ptr, &width, &height, &depth, &colorType, NULL, NULL, NULL);

    if (colorType == PNG_COLOR_TYPE_RGB)
        format = ImageFormat::RGB;
    else if (colorType == PNG_COLOR_TYPE_GRAY)
        format = ImageFormat::GRAY;
    else if (colorType == PNG_COLOR_TYPE_GRAY_ALPHA) {
        format = ImageFormat::GRAY;
        png_set_strip_alpha(png_ptr);
    }
    if (colorType == PNG_COLOR_TYPE_PALETTE && depth <= 8) {
        png_set_expand(png_ptr);
        format = ImageFormat::RGB;
    } else if (depth < 8 || png_get_valid(png_ptr, info_ptr, PNG_INFO_tRNS)) {
        png_set_expand(png_ptr);
    }
    if (depth == 16)
        png_set_strip_16(png_ptr);

    png_size_t rowBytes = png_get_rowbytes(png_ptr, info_ptr);

    StackArray(png_byte *, rows, height);
    std::vector<unsigned char> data(rowBytes * height);
    for (unsigned int i = 0; i < height; i++)
        rows[i] = &data[(height - 1 - i) * rowBytes];

    png_read_image(png_ptr, rows);

    png_destroy_read_struct(&png_ptr, &info_ptr, NULL);

    return Image(std::move(data), (int) width, (int) height, format);
}

Image Image::fromAsset(std::string name) {
    return Image::fromStream(*Resources::assets->open(name));
}