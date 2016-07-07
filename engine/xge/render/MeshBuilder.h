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
        MeshBuilder(std::shared_ptr<ShaderProgram> program, Mesh::DrawMode drawMode = Mesh::DrawMode::TRIANGLES) {
            mesh = std::shared_ptr<Mesh>(new Mesh());
            mesh->program = program;
            mesh->drawMode = drawMode;
        }

        inline void setDrawMode(Mesh::DrawMode drawMode) {
            mesh->drawMode = drawMode;
        }

        inline MeshBuilder &addAttribute(ShaderAttribute attribute, GPUBuffer::Usage usage = GPUBuffer::Usage::Static) {
            attributes.push_back(attribute);
            attributeUsages.push_back(usage);
            attributeValues.push_back({});
            return *this;
        }
        inline MeshBuilder &addAttribute(std::string attributeName, ShaderValueType valType,
                                         GPUBuffer::Usage usage = GPUBuffer::Usage::Static) {
            return addAttribute(mesh->program->getAttribute(attributeName, valType), usage);
        }

        /* wrappers for Mesh's setUniform */

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
        inline MeshBuilder &setUniform(ShaderUniform uniform, std::shared_ptr<Texture> texture) {
            mesh->setUniform(uniform, texture);
            return *this;
        }

        inline MeshBuilder &setUniform(std::string name, ShaderValueType type, std::vector<ShaderValue> value) {
            mesh->setUniform(name, type, std::move(value));
            return *this;
        }
        inline MeshBuilder &setUniform(std::string name, ShaderValueType type, ShaderValue value) {
            mesh->setUniform(name, type, value);
            return *this;
        }
        inline MeshBuilder &setUniform(std::string name, ShaderValueType type, ShaderValue *value, int count = 1) {
            mesh->setUniform(name, type, value, count);
            return *this;
        }
        inline MeshBuilder &setUniform(std::string name, std::shared_ptr<Texture> texture) {
            mesh->setUniform(name, texture);
            return *this;
        }

        /* glm type setUniform */

        /**
         * Use this function to set an uniform using an GLM type.
         */
        template <typename T>
        inline MeshBuilder &setUniform(ShaderUniform uniform, T value) {
            mesh->setUniform(uniform, (ShaderValue *) glm::value_ptr(value));
            return *this;
        }
        /**
         * Use this function to set an uniform using an GLM type.
         */
        template <typename T>
        inline MeshBuilder &setUniform(std::string name, T value) {
            mesh->setUniform(mesh->program->getUniform(name, ShaderValueTypeFromBaseType<T>()),
                             (ShaderValue *) glm::value_ptr(value));
            return *this;
        }

        /* base type setUniform */

        inline MeshBuilder &setUniform(std::string name, float value) {
            ShaderValue val;
            val.f = value;
            mesh->setUniform(name, ShaderValueType::Float, val);
            return *this;
        }
        inline MeshBuilder &setUniform(std::string name, int value) {
            ShaderValue val;
            val.i = value;
            mesh->setUniform(name, ShaderValueType::Int, val);
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