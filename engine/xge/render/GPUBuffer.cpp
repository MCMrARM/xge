#include "GPUBuffer.h"

#include <xge/opengl.h>
#include <xge/util/Log.h>

using namespace xge;

void GPUBuffer::createBuffer() {
	glGenBuffers(1, &id);
    hasId = true;
}

GPUBuffer::~GPUBuffer() {
    if (hasId)
        glDeleteBuffers(1, &id);
}

void GPUBuffer::upload(ShaderValueType type, std::vector<ShaderValue> const &data, Usage usage) {
    if (!hasId)
        createBuffer();

    glBindBuffer(GL_ARRAY_BUFFER, id);
    GLenum glUsage = (usage == Usage::Dynamic ? GL_DYNAMIC_DRAW : (usage == Usage::Stream ? GL_STREAM_DRAW : GL_STATIC_DRAW));
    glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(ShaderValue), data.data(), glUsage);
}

void GPUBuffer::uploadFragment(ShaderValueType type, unsigned int offset, std::vector<ShaderValue> const &data) {
    if (!hasId) {
        Log::error("GPUBuffer", "Cannot upload fragment: The buffer is not created yet!");
        return;
    }

    glBindBuffer(GL_ARRAY_BUFFER, id);
    glBufferSubData(GL_ARRAY_BUFFER, offset * sizeof(ShaderValue), data.size() * sizeof(ShaderValue), data.data());
}