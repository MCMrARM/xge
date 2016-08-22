#pragma once

#include <memory>
#include <vector>
#include <glm/gtc/type_ptr.hpp>
#include "Shader.h"
#include "Mesh.h"
#include "MeshBuilderConfig.h"

namespace xge {

    class MeshBuilder {

    protected:
        std::shared_ptr<Mesh> mesh;
        MeshBuilderConfig config;
        std::vector<std::vector<ShaderValue>> attributeValues;
        unsigned int vertexCount = 0;

    public:
        MeshBuilder(const MeshBuilderConfig &config, Mesh::DrawMode drawMode = Mesh::DrawMode::TRIANGLES) : config(config) {
            mesh = std::shared_ptr<Mesh>(new Mesh());
            mesh->program = config.program;
            mesh->drawMode = drawMode;
            attributeValues.resize(config.attributes.size());
        }
        MeshBuilder(std::shared_ptr<ShaderProgram> program, Mesh::DrawMode drawMode = Mesh::DrawMode::TRIANGLES) :
                config(program) {
            mesh = std::shared_ptr<Mesh>(new Mesh());
            mesh->program = program;
            mesh->drawMode = drawMode;
            attributeValues.resize(config.attributes.size());
        }

        inline void setDrawMode(Mesh::DrawMode drawMode) {
            mesh->drawMode = drawMode;
        }

        /* wrappers for MeshBuilderConfig's functions */

        inline MeshBuilder &addAttribute(ShaderAttribute attribute, MeshAttributeUsage usage = MeshAttributeUsage::CUSTOM,
                                         GPUBuffer::Usage bufferUsage = GPUBuffer::Usage::Static) {
            config.addAttribute(attribute, usage, bufferUsage);
            attributeValues.push_back({});
            return *this;
        }
        inline MeshBuilder &addAttribute(std::string attributeName, ShaderValueType valType,
                                         MeshAttributeUsage usage = MeshAttributeUsage::CUSTOM,
                                         GPUBuffer::Usage bufferUsage = GPUBuffer::Usage::Static) {
            config.addAttribute(attributeName, valType, usage, bufferUsage);
            attributeValues.push_back({});
            return *this;
        }

        inline MeshBuilder &setPrimaryTextureUniform(ShaderUniform uniform) {
            config.setPrimaryTextureUniform(uniform);
            return *this;
        }
        inline MeshBuilder &setPrimaryTextureUniform(std::string attribName) {
            config.setPrimaryTextureUniform(attribName);
            return *this;
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
#ifdef _WINDOWS
	private:
		MeshBuilder &_push(int stub, ...);

	public:
		template<typename... T>
        MeshBuilder &push(T... args) {
			return _push(0, args...);
		}
#else
        MeshBuilder &push(...);
#endif

        /**
         * This function returns how many vertex were already pushed.
         */
        inline unsigned int getVertexCount() {
            return vertexCount;
        }

        MeshBuilder &triangle(glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3, glm::vec2 uv1, glm::vec2 uv2, glm::vec2 uv3,
                      glm::vec4 color1, glm::vec4 color2, glm::vec4 color3);
        inline MeshBuilder &triangle(glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3) {
            return triangle(pos1, pos2, pos3, glm::vec2(0.f), glm::vec2(0.f), glm::vec2(0.f),
                            glm::vec4(1.f), glm::vec4(1.f), glm::vec4(1.f));
        }
        inline MeshBuilder &triangleUV(glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3,
                               glm::vec2 uv1, glm::vec2 uv2, glm::vec2 uv3) {
            return triangle(pos1, pos2, pos3, uv1, uv2, uv3, glm::vec4(1.f), glm::vec4(1.f), glm::vec4(1.f));
        }
        inline MeshBuilder &triangleColor(glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3,
                                  glm::vec4 color1, glm::vec4 color2, glm::vec4 color3) {
            return triangle(pos1, pos2, pos3, glm::vec2(0.f), glm::vec2(0.f), glm::vec2(0.f), color1, color2, color3);
        }

        // order: bottom left, top left, top right, top bottom
        inline MeshBuilder &quad(glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3, glm::vec3 pos4,
                                 glm::vec2 uv1, glm::vec2 uv2, glm::vec2 uv3, glm::vec2 uv4,
                                 glm::vec4 color1, glm::vec4 color2, glm::vec4 color3, glm::vec4 color4) {
            triangle(pos3, pos2, pos1, uv3, uv2, uv1, color3, color2, color1);
            return triangle(pos3, pos1, pos4, uv3, uv1, uv4, color3, color1, color4);
        }
        inline MeshBuilder &quad(glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3, glm::vec3 pos4,
                                 glm::vec2 uv1, glm::vec2 uv2, glm::vec2 uv3, glm::vec2 uv4, glm::vec4 color) {
            return quad(pos1, pos2, pos3, pos4, uv1, uv2, uv3, uv4, color, color, color, color);
        }
        inline MeshBuilder &quad(glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3, glm::vec3 pos4) {
            return quad(pos1, pos2, pos3, pos4, glm::vec2(0.f), glm::vec2(0.f), glm::vec2(0.f), glm::vec2(0.f),
                        glm::vec4(1.f), glm::vec4(1.f), glm::vec4(1.f), glm::vec4(1.f));
        }
        inline MeshBuilder &quadUV(glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3, glm::vec3 pos4,
                           glm::vec2 uv1, glm::vec2 uv2, glm::vec2 uv3, glm::vec2 uv4) {
            return quad(pos1, pos2, pos3, pos4, uv1, uv2, uv3, uv4,
                        glm::vec4(1.f), glm::vec4(1.f), glm::vec4(1.f), glm::vec4(1.f));
        }
        inline MeshBuilder &quadColor(glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3, glm::vec3 pos4,
                              glm::vec4 color1, glm::vec4 color2, glm::vec4 color3, glm::vec4 color4) {
            return quad(pos1, pos2, pos3, pos4, glm::vec2(0.f), glm::vec2(0.f), glm::vec2(0.f), glm::vec2(0.f),
                        color1, color2, color3, color4);
        }

        inline MeshBuilder &rect(glm::vec2 pos1, glm::vec2 pos2, glm::vec2 uv1, glm::vec2 uv2,
                           glm::vec4 color1, glm::vec4 color2, glm::vec4 color3, glm::vec4 color4) {
            return quad(glm::vec3(pos1.x, pos1.y, 0.f), glm::vec3(pos1.x, pos2.y, 0.f), glm::vec3(pos2.x, pos2.y, 0.f),
                        glm::vec3(pos2.x, pos1.y, 0.f), uv1, glm::vec2(uv1.x, uv2.y), uv2, glm::vec2(uv2.x, uv1.y),
                        color1, color2, color3, color4);
        }
        inline MeshBuilder &rect(glm::vec2 pos1, glm::vec2 pos2, glm::vec2 uv1, glm::vec2 uv2, glm::vec4 color) {
            return rect(pos1, pos2, uv1, uv2, color, color, color, color);
        }
        inline MeshBuilder &rectUV(glm::vec2 pos1, glm::vec2 pos2, glm::vec2 uv1, glm::vec2 uv2) {
            return rect(pos1, pos2, uv1, uv2, glm::vec4(1.f), glm::vec4(1.f), glm::vec4(1.f), glm::vec4(1.f));
        }
        inline MeshBuilder &rectColor(glm::vec2 pos1, glm::vec2 pos2,
                           glm::vec4 color1, glm::vec4 color2, glm::vec4 color3, glm::vec4 color4) {
            return rect(pos1, pos2, glm::vec2(0.f, 0.f), glm::vec2(0.f, 0.f), color1, color2, color3, color4);
        }
        inline MeshBuilder &rectColor(glm::vec2 pos1, glm::vec2 pos2, glm::vec4 color) {
            return rect(pos1, pos2, glm::vec2(0.f, 0.f), glm::vec2(0.f, 0.f), color, color, color, color);
        }

        // rect2 - draw the rectangle using an alternative way (useful if you use this function to draw gradients)
        inline MeshBuilder &rect2(glm::vec2 pos1, glm::vec2 pos2, glm::vec2 uv1, glm::vec2 uv2,
                                 glm::vec4 color1, glm::vec4 color2, glm::vec4 color3, glm::vec4 color4) {
            return quad(glm::vec3(pos2.x, pos1.y, 0.f), glm::vec3(pos2.x, pos2.y, 0.f), glm::vec3(pos1.x, pos2.y, 0.f),
                        glm::vec3(pos1.x, pos1.y, 0.f), glm::vec2(uv2.x, uv1.y), uv2, glm::vec2(uv1.x, uv2.y), uv1,
                        color4, color3, color2, color1);
        }

        inline MeshBuilder &setPrimaryTexture(std::shared_ptr<Texture> texture) {
            mesh->setUniform(config.primaryTextureUniform, texture);
            return *this;
        }

        /**
         * This function builds the mesh. If reuse is set to false, further use of this instance will be impossible.
         * If set to false, you'll be able to further use this object (no vertex data will be freed).
         */
        std::shared_ptr<Mesh> build(bool reuse = false);

    };

}