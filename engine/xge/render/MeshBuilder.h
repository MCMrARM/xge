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
         * This function builts the mesh and make this MeshBuilder instance unuseable.
         */
        std::shared_ptr<Mesh> build();

    };

}