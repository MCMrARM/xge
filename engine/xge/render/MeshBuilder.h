#pragma once

#include <memory>
#include <vector>
#include "Shader.h"
#include "Mesh.h"
#include <glm/gtc/type_ptr.hpp>

namespace xge {

    class MeshBuilder {

    protected:
        std::shared_ptr<Mesh> mesh;
        std::vector<ShaderAttribute> attributes;
        std::vector<GPUBuffer::Usage> attributeUsages;
        std::vector<std::vector<ShaderValue>> attributeValues;
        unsigned int vertexCount = 0;

    public:
        MeshBuilder() {
            mesh = std::shared_ptr<Mesh>(new Mesh());
        }

        inline MeshBuilder &addAttribute(ShaderAttribute attribute, GPUBuffer::Usage usage = GPUBuffer::Usage::Static) {
            attributes.push_back(attribute);
            attributeUsages.push_back(usage);
            attributeValues.push_back({});
            return *this;
        }

        inline MeshBuilder &setUniform(ShaderUniform uniform, std::vector<ShaderValue> value) {
            mesh->setUniform(uniform, std::move(value));
            return *this;
        }
        inline MeshBuilder &setUniform(ShaderUniform uniform, ShaderValue value) {
            mesh->setUniform(uniform, value);
            return *this;
        }
        inline MeshBuilder &setUniform(ShaderUniform uniform, ShaderValue *value, int count = 1) {
            mesh->setUniform(uniform, value, count);
            return *this;
        }
        inline MeshBuilder &setUniform(ShaderUniform uniform, Texture *texture) {
            mesh->setUniform(uniform, texture);
            return *this;
        }
        /**
         * Use this function to set an uniform using an GLM type.
         */
        template <typename T>
        inline MeshBuilder &setUniform(ShaderUniform uniform, T value) {
            mesh->setUniform(uniform, (ShaderValue *) glm::value_ptr(value));
            return *this;
        }

        /**
         * This function pushes a vertex onto the list. The argument count should equal the count of attributes.
         */
        MeshBuilder &push(...);

        /**
         * This function builds the mesh. If reuse is set to false, further use of this instance will be impossible.
         * If set to false, you'll be able to further use this object (no vertex data will be freed).
         */
        std::shared_ptr<Mesh> build(bool reuse = false);

    };

}