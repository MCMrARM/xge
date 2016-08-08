#include "Shader.h"
#include "GLError.h"

#include <xge/opengl.h>
#include <xge/util/Resources.h>
#include <xge/util/Log.h>
#include <xge/util/DynamicStackArray.h>
#include "Texture.h"
#include "VertexAttribArrayManager.h"

using namespace xge;

ShaderProgram *ShaderProgram::current;

Shader::Shader(ShaderType type) : type(type) {
    GLenum shaderType = 0;
    switch (type) {
        case ShaderType::Vertex:
            shaderType = GL_VERTEX_SHADER;
            break;
        case ShaderType::Fragment:
            shaderType = GL_FRAGMENT_SHADER;
            break;
#ifdef GL_COMPUTE_SHADER
        case ShaderType::Compute:
            shaderType = GL_COMPUTE_SHADER;
            break;
#endif
#ifdef GL_GEOMETRY_SHADER
        case ShaderType::Geometry:
            shaderType = GL_GEOMETRY_SHADER;
            break;
#endif
#ifdef GL_TESS_CONTROL_SHADER
        case ShaderType::TessControl:
            shaderType = GL_TESS_CONTROL_SHADER;
            break;
#endif
#ifdef GL_TESS_EVALUATION_SHADER
        case ShaderType::TessEvaluation:
            shaderType = GL_TESS_EVALUATION_SHADER;
            break;
#endif
    }
    id = glCreateShader(shaderType);
    if (id == 0)
        throw GLError();
}

Shader::~Shader() {
    if (id != 0)
        glDeleteShader(id);
}

void Shader::setSource(int sourceCount, const char **sources, int *sourceLengths) {
    glShaderSource(id, sourceCount, sources, sourceLengths);
}

void Shader::setSource(std::vector<std::string> sources) {
    StackArray(const char *, pSources, sources.size());
	StackArray(int, pSourcesLengths, sources.size());
    int i = 0;
    for (auto it = sources.begin(); it != sources.end(); it++) {
        pSources[i] = it->data();
        pSourcesLengths[i++] = (int) it->size();
    }
    setSource((int) sources.size(), pSources, pSourcesLengths);
}

void Shader::setSource(std::vector<std::vector<char>> sources) {
	StackArray(const char *, pSources, sources.size());
	StackArray(int, pSourcesLengths, sources.size());
    int i = 0;
    for (auto it = sources.begin(); it != sources.end(); it++) {
        pSources[i] = it->data();
        pSourcesLengths[i++] = (int) it->size();
    }
    setSource((int) sources.size(), pSources, pSourcesLengths);
}

void Shader::compile() {
    glCompileShader(id);
    GLint compileStatus;
    glGetShaderiv(id, GL_COMPILE_STATUS, &compileStatus);
    if (compileStatus == GL_FALSE) {
        throw ShaderCompileError(getInfoLog());
    }
}

std::string Shader::getInfoLog() {
    GLint logLength;
    glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logLength);
    std::string log;
    log.resize((size_t) logLength);
    glGetShaderInfoLog(id, logLength, nullptr, &log[0]);
    return log;
}

Shader Shader::createFromAssets(ShaderType type, std::string fileName) {
    Shader ret (type);
    ret.setSource(Resources::assets->readFile(fileName));
    ret.compile();
    return std::move(ret);
}

ShaderProgram::ShaderProgram() {
    id = glCreateProgram();
    if (id == 0)
        throw GLError();
}

ShaderProgram::~ShaderProgram() {
    if (current == this)
        current = nullptr;
    glDeleteProgram(id);
}

void ShaderProgram::attach(Shader const &shader) {
    glAttachShader(id, shader.getId());
}

void ShaderProgram::link() {
    glLinkProgram(id);
    GLint linkStatus;
    glGetProgramiv(id, GL_LINK_STATUS, &linkStatus);
    if (linkStatus == GL_FALSE) {
        throw ProgramLinkError(getInfoLog());
    }
}

std::string ShaderProgram::getInfoLog() {
    GLint logLength;
    glGetProgramiv(id, GL_INFO_LOG_LENGTH, &logLength);
    std::string log;
    log.resize((size_t) logLength);
    glGetProgramInfoLog(id, logLength, nullptr, &log[0]);
    return log;
}

ShaderAttribute ShaderProgram::getAttribute(std::string name, ShaderValueType type) {
    if (attributes.count(name) > 0) {
        return { attributes.at(name), type };
    }
    use();
    int attribLoc = glGetAttribLocation(id, name.c_str());
    if (attribLoc == -1)
        throw GLError();
    VertexAttribArrayManager::instance.enable(attribLoc);
    attributes[name] = attribLoc;
    vertexAttribIds.insert(attribLoc);
    return { attribLoc, type };
}

ShaderUniform ShaderProgram::getUniform(std::string name, ShaderValueType type) {
    if (uniforms.count(name) > 0) {
        return { uniforms.at(name), type };
    }
    use();
    int uniformLoc = glGetUniformLocation(id, name.c_str());
    if (uniformLoc == -1)
        throw GLError();
    uniforms[name] = uniformLoc;
    return { uniformLoc, type };
}

void ShaderProgram::use() {
    if (id == 0 || current == this)
        return;
    glUseProgram(id);
    VertexAttribArrayManager::instance.set(vertexAttribIds);
    current = this;
}

void ShaderUniform::set(ShaderValue *value, int count) {
    switch (type) {
        case ShaderValueType::Float:
            glUniform1fv(uniformId, count, (float*) value);
            break;
        case ShaderValueType::Vec2:
            glUniform2fv(uniformId, count, (float*) value);
            break;
        case ShaderValueType::Vec3:
            glUniform3fv(uniformId, count, (float*) value);
            break;
        case ShaderValueType::Vec4:
            glUniform4fv(uniformId, count, (float*) value);
            break;
        case ShaderValueType::Int:
            glUniform1iv(uniformId, count, (int*) value);
            break;
        case ShaderValueType::IVec2:
            glUniform2iv(uniformId, count, (int*) value);
            break;
        case ShaderValueType::IVec3:
            glUniform3iv(uniformId, count, (int*) value);
            break;
        case ShaderValueType::IVec4:
            glUniform4iv(uniformId, count, (int*) value);
            break;
#ifdef glUniform1uiv
        case ShaderValueType::UInt:
            glUniform1uiv(uniformId, count, (unsigned int*) value);
            break;
        case ShaderValueType::UIVec2:
            glUniform2uiv(uniformId, count, (unsigned int*) value);
            break;
        case ShaderValueType::UIVec3:
            glUniform3uiv(uniformId, count, (unsigned int*) value);
            break;
        case ShaderValueType::UIVec4:
            glUniform4uiv(uniformId, count, (unsigned int*) value);
            break;
#endif
        case ShaderValueType::Matrix2:
            glUniformMatrix2fv(uniformId, count, GL_FALSE, (float*) value);
            break;
        case ShaderValueType::Matrix3:
            glUniformMatrix3fv(uniformId, count, GL_FALSE, (float*) value);
            break;
        case ShaderValueType::Matrix4:
            glUniformMatrix4fv(uniformId, count, GL_FALSE, (float*) value);
            break;
        default:
            Log::error("ShaderUniform", "Unsupported value type.");
    }
}

void ShaderUniform::set(Texture *value) {
    int bindId = value->getBindId();
#ifndef NDEBUG
    if (bindId == -1) {
        Log::error("ShaderUniform", "Cannot set texture: it's not bound.");
        return;
    }
#endif
    glUniform1i(uniformId, bindId);
}