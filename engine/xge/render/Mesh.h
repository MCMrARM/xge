#pragma once

#include <unordered_map>
#include <vector>
#include <memory>
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
            std::shared_ptr<Texture> texture;
        };

        enum class DrawMode {
            TRIANGLES, TRIANGLE_STRIP, TRIANGLE_FAN, POINTS, LINES, LINE_STRIP, LINE_LOOP
        };

        std::shared_ptr<ShaderProgram> program;
        unsigned int vertexCount = 0;
        std::unordered_map<int, AttributeValue> attributes;
        std::unordered_map<int, UniformValue> uniforms;
        DrawMode drawMode = DrawMode::TRIANGLES;

        inline void setUniform(ShaderUniform uniform, std::vector<ShaderValue> value) {
            uniforms[uniform.uniformId] = { uniform, std::move(value) };
        }
        inline void setUniform(ShaderUniform uniform, ShaderValue value) {
            uniforms[uniform.uniformId] = { uniform, { value } };
        }
        void setUniform(ShaderUniform uniform, ShaderValue *value, int count = 1); // This will copy the array specified in parameter.
        void setUniform(ShaderUniform uniform, std::shared_ptr<Texture> texture);

        inline void setUniform(std::string name, ShaderValueType type, std::vector<ShaderValue> value) {
            setUniform(program->getUniform(name, type), std::move(value));
        }
        inline void setUniform(std::string name, ShaderValueType type, ShaderValue value) {
            setUniform(program->getUniform(name, type), value);
        }
        inline void setUniform(std::string name, ShaderValueType type, ShaderValue *value, int count = 1) {
            setUniform(program->getUniform(name, type), value, count);
        }
        inline void setUniform(std::string name, std::shared_ptr<Texture> texture) {
            setUniform(program->getUniform(name, ShaderValueType::Sampler2D), std::move(texture));
        }

        void draw();

    };

}