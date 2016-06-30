#include "GLError.h"

#include <xge/opengl.h>
#include <sstream>

using namespace xge;

GLError::GLError() {
    errorCode = glGetError();
    errorStr = "Unknown"; // TODO
    buildErrorStr();
}

void GLError::buildErrorStr() {
    std::stringstream ss;
    ss << "OpenGL Error: " << errorStr << " (" << errorCode << ")";
    throwStr = ss.str();
}