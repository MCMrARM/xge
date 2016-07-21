#pragma once

#include <memory>
#include <vector>
#include "Shader.h"
#include "Mesh.h"
#include <glm/gtc/type_ptr.hpp>

namespace xge {

    class MeshBuilder {

    public:
        enum class AttributeUsage {
            POSITION, COLOR, TEXTURE_UV, CUSTOM
        };

    protected:
        std::shared_ptr<Mesh> mesh;
        std::vector<ShaderAttribute> attributes;
        std::vector<AttributeUsage> attributeUsages;
        std::vector<GPUBuffer::Usage> attributeBufferUsages;
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

        /**
         * Add a uniform to the builder. This method should not be called if any vertex was already pushed to the list.
         * The attribute usage is how you use the attribute in shader. Please use the correct one and if you don't see
         * your usage on the list use AttributeUsage::CUSTOM. If you don't specify the attribute usage or use
         * AttributeUsage::CUSTOM, you'll not be able to call ::vertex() or ::quad().
         */
        inline MeshBuilder &addAttribute(ShaderAttribute attribute, AttributeUsage usage = AttributeUsage::CUSTOM,
                                         GPUBuffer::Usage bufferUsage = GPUBuffer::Usage::Static) {
            attributes.push_back(attribute);
            attributeUsages.push_back(usage);
            attributeBufferUsages.push_back(bufferUsage);
            attributeValues.push_back({});
            return *this;
        }
        inline MeshBuilder &addAttribute(std::string attributeName, ShaderValueType valType,
                                         AttributeUsage usage = AttributeUsage::CUSTOM,
                                         GPUBuffer::Usage bufferUsage = GPUBuffer::Usage::Static) {
            return addAttribute(mesh->program->getAttribute(attributeName, valType), usage, bufferUsage);
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

        void triangle(glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3, glm::vec2 uv1, glm::vec2 uv2, glm::vec2 uv3,
                      glm::vec4 color1, glm::vec4 color2, glm::vec4 color3);
        inline void triangle(glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3) {
            triangle(pos1, pos2, pos3, glm::vec2(0.f), glm::vec2(0.f), glm::vec2(0.f), glm::vec4(1.f), glm::vec4(1.f),
                     glm::vec4(1.f));
        }
        inline void triangleUV(glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3,
                               glm::vec2 uv1, glm::vec2 uv2, glm::vec2 uv3) {
            triangle(pos1, pos2, pos3, uv1, uv2, uv3, glm::vec4(1.f), glm::vec4(1.f), glm::vec4(1.f));
        }
        inline void triangleColor(glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3,
                                  glm::vec4 color1, glm::vec4 color2, glm::vec4 color3) {
            triangle(pos1, pos2, pos3, glm::vec2(0.f), glm::vec2(0.f), glm::vec2(0.f), color1, color2, color3);
        }

        // order: bottom left, top left, top right, top bottom
        void quad(glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3, glm::vec3 pos4,
                  glm::vec2 uv1, glm::vec2 uv2, glm::vec2 uv3, glm::vec2 uv4,
                  glm::vec4 color1, glm::vec4 color2, glm::vec4 color3, glm::vec4 color4);
        inline void quad(glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3, glm::vec3 pos4) {
            quad(pos1, pos2, pos3, pos4, glm::vec2(0.f), glm::vec2(0.f), glm::vec2(0.f), glm::vec2(0.f),
                 glm::vec4(1.f), glm::vec4(1.f), glm::vec4(1.f), glm::vec4(1.f));
        }
        inline void quadUV(glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3, glm::vec3 pos4,
                           glm::vec2 uv1, glm::vec2 uv2, glm::vec2 uv3, glm::vec2 uv4) {
            quad(pos1, pos2, pos3, pos4, uv1, uv2, uv3, uv4, glm::vec4(1.f), glm::vec4(1.f), glm::vec4(1.f), glm::vec4(1.f));
        }
        inline void quadColor(glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3, glm::vec3 pos4,
                              glm::vec4 color1, glm::vec4 color2, glm::vec4 color3, glm::vec4 color4) {
            quad(pos1, pos2, pos3, pos4, glm::vec2(0.f), glm::vec2(0.f), glm::vec2(0.f), glm::vec2(0.f),
                 color1, color2, color3, color4);
        }

        inline void rect(glm::vec2 pos1, glm::vec2 pos2, glm::vec2 uv1, glm::vec2 uv2,
                           glm::vec4 color1, glm::vec4 color2, glm::vec4 color3, glm::vec4 color4) {
            quad(glm::vec3(pos1.x, pos1.y, 0.f), glm::vec3(pos1.x, pos2.y, 0.f), glm::vec3(pos2.x, pos2.y, 0.f),
                 glm::vec3(pos2.x, pos1.y, 0.f), uv1, glm::vec2(uv1.x, uv2.y), uv2, glm::vec2(uv2.x, uv1.y),
                 color1, color2, color3, color4);
        }
        inline void rectUV(glm::vec2 pos1, glm::vec2 pos2, glm::vec2 uv1, glm::vec2 uv2) {
            rect(pos1, pos2, uv1, uv2, glm::vec4(1.f), glm::vec4(1.f), glm::vec4(1.f), glm::vec4(1.f));
        }
        inline void rectColor(glm::vec2 pos1, glm::vec2 pos2,
                           glm::vec4 color1, glm::vec4 color2, glm::vec4 color3, glm::vec4 color4) {
            rect(pos1, pos2, glm::vec2(0.f, 0.f), glm::vec2(0.f, 0.f), color1, color2, color3, color4);
        }


        /**
         * This function builds the mesh. If reuse is set to false, further use of this instance will be impossible.
         * If set to false, you'll be able to further use this object (no vertex data will be freed).
         */
        std::shared_ptr<Mesh> build(bool reuse = false);

    };

}