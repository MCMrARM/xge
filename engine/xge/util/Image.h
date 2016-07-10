#pragma once

#include <string>
#include <vector>
#include <cstdlib>
#include "Log.h"

namespace xge {

    class InputStream;

    enum class ImageFormat {
        GRAY, RGB, RGBA
    };

    struct ImagePixelColor {
        unsigned char a, r, g, b;
    };

    /**
     * This class stores data of some image. It doesn't upload the image to GPU (see Texture if you want to upload some
     * image to GPU and use it in rendering code).
     */
    class Image {

    private:
        std::vector<unsigned char> data;
        int w = 0, h = 0;
        ImageFormat format = ImageFormat::RGBA;

    public:
        Image() {
            //
        }
        Image(std::vector<unsigned char> data, int w, int h, ImageFormat format) {
            setData(std::move(data), w, h, format);
        }

        inline void setData(std::vector<unsigned char> data, int w, int h, ImageFormat format) {
            this->data = std::move(data);
            this->w = w;
            this->h = h;
            this->format = format;
        }

        inline std::vector<unsigned char> &getData() {
            return data;
        }
        inline std::vector<unsigned char> const &getData() const {
            return data;
        }
        inline int getWidth() const {
            return w;
        }
        inline int getHeight() const {
            return h;
        }
        inline ImageFormat getFormat() const {
            return format;
        }

        /**
         * See getPixel() for details.
         */
        inline unsigned char getPixelGray(int x, int y) {
            XGEAssert(format == ImageFormat::GRAY);
            return data[y * w + x];
        }
        /**
         * See getPixel() for details.
         */
        inline ImagePixelColor getPixelRGB(int x, int y) {
            XGEAssert(format == ImageFormat::RGB);
            unsigned char *p = &data[(y * w + x) * 3];
            return {255, p[0], p[1], p[2]};
        }
        /**
         * See getPixel() for details.
         */
        inline ImagePixelColor getPixelRGBA(int x, int y) {
            XGEAssert(format == ImageFormat::RGBA);
            unsigned char *p = &data[(y * w + x) * 4];
            return {p[3], p[0], p[1], p[2]};
        }
        /**
         * This function will return the color of the pixel at the specified coords. This function may be a bit slow
         * when used many times so if you are sure about the pixel format, use getPixelMono, getPixelRGB or getPixelRGBA.
         */
        inline ImagePixelColor getPixel(int x, int y) {
            switch (format) {
                case ImageFormat::GRAY: {
                    unsigned char c = getPixelGray(x, y);
                    return {c, c, c, c};
                }
                case ImageFormat::RGB:
                    return getPixelRGB(x, y);
                case ImageFormat::RGBA:
                    return getPixelRGBA(x, y);
            }
        }

        static Image fromStream(InputStream &stream);

        static Image fromAsset(std::string name);

    };

}



