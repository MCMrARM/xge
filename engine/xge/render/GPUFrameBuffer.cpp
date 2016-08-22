#include "GPUFrameBuffer.h"

#include <xge/opengl.h>
#include <xge/util/Log.h>
#include "Texture.h"

using namespace xge;

GPUFrameBuffer *GPUFrameBuffer::boundFramebuffer = nullptr;

unsigned int xge::GetGLFBOAttachmentTypeVal(GPUFrameBufferAttachmentType type) {
    switch (type) {
        case GPUFrameBufferAttachmentType::COLOR:
            return GL_COLOR_ATTACHMENT0;
        case GPUFrameBufferAttachmentType::DEPTH:
            return GL_DEPTH_ATTACHMENT;
        case GPUFrameBufferAttachmentType::STENCIL:
            return GL_STENCIL_ATTACHMENT;
    }
    return GL_COLOR_ATTACHMENT0;
}

GPUFrameBuffer::GPUFrameBuffer() {
    glGenFramebuffers(1, &id);
}

GPUFrameBuffer::~GPUFrameBuffer() {
    unbind();
    glDeleteFramebuffers(1, &id);
}

void GPUFrameBuffer::attach(GPUFrameBufferAttachmentType type, std::shared_ptr<GPURenderBuffer> buffer) {
    XGEAssert(isBound());
    buffers.push_back(buffer);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GetGLFBOAttachmentTypeVal(type), GL_RENDERBUFFER, buffer->getId());
}

void GPUFrameBuffer::attach(GPUFrameBufferAttachmentType type, Texture &texture) {
    XGEAssert(isBound());
    glFramebufferTexture2D(GL_FRAMEBUFFER, GetGLFBOAttachmentTypeVal(type), GL_TEXTURE_2D, texture.getId(), 0);
}

void GPUFrameBuffer::bind() {
    if (!isBound()) {
        boundFramebuffer = this;
        glBindFramebuffer(GL_FRAMEBUFFER, id);
    }
}

void GPUFrameBuffer::unbind() {
    if (isBound()) {
        boundFramebuffer = nullptr;
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}