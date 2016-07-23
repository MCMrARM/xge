#pragma once

#include <memory>
#include "Shader.h"
#include "GPUBuffer.h"

namespace xge {

    enum class MeshAttributeUsage {
        POSITION, COLOR, TEXTURE_UV, CUSTOM
    };

    class MeshBuilderConfig {

    protected:
        friend class MeshBuilder;
        friend class MeshEditor;

        std::shared_ptr<ShaderProgram> program;
        std::vector<ShaderAttribute> attributes;
        std::vector<MeshAttributeUsage> attributeUsages;
        std::vector<GPUBuffer::Usage> attributeBufferUsages;
        ShaderUniform primaryTextureUniform;

    public:
        MeshBuilderConfig() {
            //
        }
        MeshBuilderConfig(std::shared_ptr<ShaderProgram> program) : program(program) {
            //
        }

        /**
         * Add a uniform to the builder config. This method should not be called if any vertex was already pushed to the
         * list. The attribute usage is how you use the attribute in shader. Please use the correct one and if you don't
         * see your usage on the list use AttributeUsage::CUSTOM. If you don't specify the attribute usage or use
         * AttributeUsage::CUSTOM, you'll not be able to call MeshBuilder::vertex() or MeshBuilder::quad().
         */
        inline MeshBuilderConfig &addAttribute(ShaderAttribute attribute,
                                               MeshAttributeUsage usage = MeshAttributeUsage::CUSTOM,
                                               GPUBuffer::Usage bufferUsage = GPUBuffer::Usage::Static) {
            attributes.push_back(attribute);
            attributeUsages.push_back(usage);
            attributeBufferUsages.push_back(bufferUsage);
            return *this;
        }
        inline MeshBuilderConfig &addAttribute(std::string attributeName, ShaderValueType valType,
                                               MeshAttributeUsage usage = MeshAttributeUsage::CUSTOM,
                                               GPUBuffer::Usage bufferUsage = GPUBuffer::Usage::Static) {
            return addAttribute(program->getAttribute(attributeName, valType), usage, bufferUsage);
        }

        inline MeshBuilderConfig &setPrimaryTextureUniform(ShaderUniform uniform) {
            primaryTextureUniform = uniform;
            return *this;
        }
        inline MeshBuilderConfig &setPrimaryTextureUniform(std::string attribName) {
            primaryTextureUniform = program->getUniform(attribName, ShaderValueType::Sampler2D);
            return *this;
        }

    };

}