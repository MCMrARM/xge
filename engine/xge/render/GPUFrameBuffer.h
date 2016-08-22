#pragma once

#include <memory>
#include <vector>
#include "GPURenderBuffer.h"

namespace xge {

    enum class GPUFrameBufferAttachmentType {
        COLOR, DEPTH, STENCIL
    };

    static unsigned int GetGLFBOAttachmentTypeVal(GPUFrameBufferAttachmentType type);

    class Texture;

    class GPUFrameBuffer {

    private:
        static GPUFrameBuffer *boundFramebuffer;

        unsigned int id;

        std::vector<std::shared_ptr<GPURenderBuffer>> buffers;

    public:
        GPUFrameBuffer();
        ~GPUFrameBuffer();

        /**
         * Attaches a renderbuffer to the framebuffer. Requires that this framebuffer is bound (using bind).
         */
        void attach(GPUFrameBufferAttachmentType type, std::shared_ptr<GPURenderBuffer> buffer);

        void attach(GPUFrameBufferAttachmentType type, Texture &texture);

        void bind();
        void unbind();

        inline bool isBound() const { return boundFramebuffer == this; }

    };

}