#pragma once

#include <unordered_map>
#include <vector>
#include "Shader.h"
#include "GPUBuffer.h"

namespace xge {

    class Mesh {

    public:
        struct AttributeValue {
            ShaderAttribute attribute;
            GPUBuffer buffer;
        };
        struct UniformValue {
            ShaderUniform uniform;
            std::vector<ShaderValue> value;
            Texture *texture;
        };

        enum class DrawMode {
            TRIANGLES, TRIANGLE_STRIP, TRAINGLE_FAN, POINTS, LINES, LINE_STRIP, LINE_LOOP
        };

        unsigned int vertexCount = 0;
        std::unordered_map<int, AttributeValue> attributes;
        std::unordered_map<int, UniformValue> uniforms;

        inline void setUniform(ShaderUniform uniform, std::vector<ShaderValue> value) {
            uniforms[uniform.uniformId] = { uniform, std::move(value) };
        }
        inline void setUniform(ShaderUniform uniform, ShaderValue value) {
            uniforms[uniform.uniformId] = { uniform, { value } };
        }
        void setUniform(ShaderUniform uniform, ShaderValue *value, int count = 1); // This will copy the array specified in parameter.
        void setUniform(ShaderUniform uniform, Texture *texture);

        void draw();

    };

}