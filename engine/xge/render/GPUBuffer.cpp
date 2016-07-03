#include "GPUBuffer.h"

#include <xge/opengl.h>
#include "../util/Log.h"

using namespace xge;

void GPUBuffer::createBuffer() {
    glCreateBuffers(1, &id);
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