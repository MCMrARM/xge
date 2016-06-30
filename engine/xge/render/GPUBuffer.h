#pragma once

#include <vector>
#include "Shader.h"

namespace xge {

    class GPUBuffer {

    private:
        bool hasId = false;
        unsigned int id = 0;

        void createBuffer();

    public:
        enum class Usage {
            Static, Dynamic, Stream
        };

        GPUBuffer() { }
        GPUBuffer(GPUBuffer const &) = delete;
        GPUBuffer(GPUBuffer &&b) : hasId(b.hasId), id(b.id) {
            b.hasId = false;
            b.id = 0;
        }
        ~GPUBuffer();

        GPUBuffer& operator=(GPUBuffer &&b) {
            hasId = b.hasId;
            id = b.id;
            b.hasId = false;
            b.id = 0;
        }

        inline unsigned int getId() {
            if (!hasId)
                createBuffer();
            return id;
        }

        void upload(ShaderValueType type, std::vector<ShaderValue> const &data, Usage usage = Usage::Static);

    };

}