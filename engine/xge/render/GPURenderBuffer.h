#pragma once

namespace xge {

    enum class GPURenderBufferFormat {
        RGBA, // RGBA = RGBA8, TODO: Fallback to RGBA4 on unsupported devices
        RGBA4, RGB565, RGB5_A1, DEPTH16, STENCIL8
    };

    class GPURenderBuffer {

    private:
        unsigned int id;

    public:
        GPURenderBuffer(int w, int h, GPURenderBufferFormat format = GPURenderBufferFormat::RGBA);

        ~GPURenderBuffer();

        inline unsigned int getId() { return id; }

    };

}