#include "GPURenderBuffer.h"

#include <xge/opengl.h>

using namespace xge;

GPURenderBuffer::GPURenderBuffer(int w, int h, GPURenderBufferFormat format) {
    glGenRenderbuffers(1, &id);
    glBindRenderbuffer(GL_RENDERBUFFER, id);
    GLenum glFormat;
    switch (format) {
        case GPURenderBufferFormat::RGBA:
            glFormat = GL_RGBA;
            break;
        case GPURenderBufferFormat::RGBA4:
            glFormat = GL_RGBA4;
            break;
        case GPURenderBufferFormat::RGB565:
            glFormat = GL_RGB565;
            break;
        case GPURenderBufferFormat::RGB5_A1:
            glFormat = GL_RGB5_A1;
            break;
        case GPURenderBufferFormat::DEPTH16:
            glFormat = GL_DEPTH_COMPONENT16;
            break;
        case GPURenderBufferFormat::STENCIL8:
            glFormat = GL_STENCIL_INDEX8;
            break;
    }
    glRenderbufferStorage(GL_RENDERBUFFER, glFormat, w, h);
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}

GPURenderBuffer::~GPURenderBuffer() {
    glDeleteRenderbuffers(1, &id);
}