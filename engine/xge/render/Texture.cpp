#include "Texture.h"

#include <xge/opengl.h>
#include <xge/util/Log.h>
#include <xge/util/Image.h>
#include "GLError.h"

using namespace xge;

Texture::Texture(Image const &image) {
    TextureFormat format;
    TextureInternalFormat internalFormat;
    switch (image.getFormat()) {
        case ImageFormat::GRAY:
            format = TextureFormat::ALPHA;
            internalFormat = TextureInternalFormat::ALPHA;
            break;
        case ImageFormat::RGB:
            format = TextureFormat::RGB;
            internalFormat = TextureInternalFormat::RGB;
            break;
        case ImageFormat::RGBA:
            format = TextureFormat::RGBA;
            internalFormat = TextureInternalFormat::RGBA;
            break;
    }
    upload(internalFormat, format, (char *) image.getData().data(), image.getWidth(), image.getHeight(), 0);
}

Texture::~Texture() {
    if (hasId) {
        glDeleteTextures(1, &id);
        hasId = false;
    }
}

void Texture::createTexture() {
    glGenTextures(1, &id);
    hasId = true;
}

void Texture::upload(TextureInternalFormat internalFormat, TextureFormat format, char *data, int w, int h, int level) {
    if (!hasId)
        createTexture();
    GLenum glInternalFormat = 0, glFormat = 0, glType = GL_UNSIGNED_BYTE;
    switch (internalFormat) {
        case TextureInternalFormat::ALPHA:
            glInternalFormat = GL_ALPHA;
            break;
        case TextureInternalFormat::RGB:
            glInternalFormat = GL_RGB;
            break;
        case TextureInternalFormat::RGBA:
            glInternalFormat = GL_RGBA;
            break;
        case TextureInternalFormat::LUMINANCE:
            glInternalFormat = GL_LUMINANCE;
            break;
        case TextureInternalFormat::LUMINANCE_ALPHA:
            glInternalFormat = GL_LUMINANCE_ALPHA;
    }
    switch (format) {
        case TextureFormat::ALPHA:
            glFormat = GL_ALPHA;
            break;
        case TextureFormat::RGB:
            glFormat = GL_RGB;
            break;
        case TextureFormat::RGB565:
            glFormat = GL_RGB;
            glType = GL_UNSIGNED_SHORT_5_6_5;
            break;
        case TextureFormat::RGBA:
            glFormat = GL_RGBA;
            break;
        case TextureFormat::RGBA4444:
            glFormat = GL_RGBA;
            glType = GL_UNSIGNED_SHORT_4_4_4_4;
            break;
        case TextureFormat::RGBA5551:
            glFormat = GL_RGBA;
            glType = GL_UNSIGNED_SHORT_5_5_5_1;
            break;
    }
    glBindTexture(GL_TEXTURE_2D, id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, level, glInternalFormat, w, h, 0, glFormat, glType, data);
#ifndef NDEBUG
    GLError::checkAndThrow();
#endif
}

void Texture::upload(TextureFormat format, char *data, int w, int h, int level) {
    TextureInternalFormat internalFormat;
    switch (format) {
        case TextureFormat::ALPHA:
            internalFormat = TextureInternalFormat::ALPHA;
            break;
        case TextureFormat::RGB:
        case TextureFormat::RGB565:
            internalFormat = TextureInternalFormat::RGB;
            break;
        default:
            internalFormat = TextureInternalFormat::RGBA;
    }
    upload(internalFormat, format, data, w, h, level);
}

void Texture::bind(int texId) {
#ifndef NDEBUG
    if (bindId != -1) {
        Log::warn("Texture", "Texture already bound to: %i", bindId);
    }
#endif
    bindId = texId;
    glActiveTexture((GLenum) (GL_TEXTURE0 + texId));
    glBindTexture(GL_TEXTURE_2D, id);
}

void Texture::unbind() {
#ifndef NDEBUG
    if (bindId == -1) {
        Log::debug("Texture", "Texture not bound, cannot unbind.");
    }
#endif
    bindId = -1;
}