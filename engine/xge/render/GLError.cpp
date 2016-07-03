#include "GLError.h"

#include <xge/opengl.h>
#include <sstream>

using namespace xge;

GLError::GLError() {
    errorCode = glGetError();
    setErrorDescFromCode();
    buildErrorStr();
}

void GLError::setErrorDescFromCode() {
    errorStr = "Unknown"; // TODO
}

void GLError::buildErrorStr() {
    std::stringstream ss;
    ss << "OpenGL Error: " << errorStr << " (" << errorCode << ")";
    throwStr = ss.str();
}

void GLError::checkAndThrow() {
    int err = glGetError();
    if (err != 0)
        throw GLError(err);
}