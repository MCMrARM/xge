#include "Mesh.h"

#include <xge/opengl.h>

using namespace xge;

void Mesh::draw() {
    for (auto &p : attributes) {
        glBindBuffer(GL_ARRAY_BUFFER, p.second.buffer.getId());
        glVertexAttribPointer((GLuint) p.second.attribute.attributeId, GetShaderValueTypeComponentCount(p.second.attribute.type), GL_FLOAT, GL_FALSE, 0, 0);
    }
    for (auto &p : uniforms) {
        p.second.uniform.set(p.second.value.data(), (int) (p.second.value.size() / GetShaderValueTypeComponentCount(p.second.uniform.type)));
    }
    glDrawArrays(GL_TRIANGLES, 0, vertexCount);
}