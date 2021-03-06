#include "VertexAttribArrayManager.h"

#include <xge/opengl.h>

using namespace xge;

VertexAttribArrayManager VertexAttribArrayManager::instance;

void VertexAttribArrayManager::enable(int id) {
    if (enabled.count(id) <= 0) {
        enabled.insert(id);
        glEnableVertexAttribArray((GLuint) id);
    }
}

void VertexAttribArrayManager::disable(int id) {
    if (enabled.count(id) > 0) {
        enabled.erase(id);
        glDisableVertexAttribArray((GLuint) id);
    }
}

void VertexAttribArrayManager::disableOpenGL(int id) {
	glDisableVertexAttribArray((GLuint)id);
}
