#include "Mesh.h"

#include <xge/opengl.h>
#include <cstring>
#include "Texture.h"

using namespace xge;

void Mesh::draw() {
    for (auto &p : attributes) {
        glBindBuffer(GL_ARRAY_BUFFER, p.second.buffer.getId());
        glVertexAttribPointer((GLuint) p.second.attribute.attributeId, GetShaderValueTypeComponentCount(p.second.attribute.type), GL_FLOAT, GL_FALSE, 0, 0);
    }
    int nextTexId = 0;
    for (auto &p : uniforms) {
        if (p.second.texture != nullptr) {
            p.second.texture->bind(nextTexId++);
            p.second.uniform.set(p.second.texture);
        } else
            p.second.uniform.set(p.second.value.data(), (int) (p.second.value.size() / GetShaderValueTypeComponentCount(p.second.uniform.type)));
    }
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
    for (auto &p : uniforms) {
        if (p.second.texture != nullptr) {
            p.second.texture->unbind();
        }
    }
}

void Mesh::setUniform(ShaderUniform uniform, ShaderValue *value, int count) {
    std::vector<ShaderValue> val((size_t) (count * GetShaderValueTypeComponentCount(uniform.type)));
    memcpy(val.data(), value, val.size() * sizeof(ShaderValue));
    uniforms[uniform.uniformId] = { uniform, std::move(val), nullptr };
}

void Mesh::setUniform(ShaderUniform uniform, Texture *texture) {
    uniforms[uniform.uniformId] = { uniform, std::vector<ShaderValue>(), texture };
}