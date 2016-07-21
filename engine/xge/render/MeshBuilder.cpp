#include "MeshBuilder.h"

#include <stdarg.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <xge/util/Log.h>

using namespace xge;

#define pushValTypeFloat(vector, val) { \
        ShaderValue shaderValType; \
        shaderValType.f = val; \
        vector->push_back(shaderValType); \
    }
#define pushValTypeVec2(vector, val) { \
        pushValTypeFloat(vector, val.x); \
        pushValTypeFloat(vector, val.y); \
    }
#define pushValTypeVec3(vector, val) { \
        pushValTypeVec2(vector, val); \
        pushValTypeFloat(vector, val.z); \
    }
#define pushValTypeVec4(vector, val) { \
        pushValTypeVec3(vector, val); \
        pushValTypeFloat(vector, val.w); \
    }

MeshBuilder &MeshBuilder::push(...) {
    va_list args;
    va_start(args, this);
    vertexCount++;
    auto itV = attributeValues.begin();
    for (auto it = attributes.begin(); it != attributes.end() && itV != attributeValues.end(); it++, itV++) {
        switch (it->type) {
            case ShaderValueType::Float:
                pushValTypeFloat(itV, va_arg(args, double));
                break;
            case ShaderValueType::Vec2: {
                auto val = va_arg(args, glm::vec2);
                pushValTypeVec2(itV, val);
            }
                break;
            case ShaderValueType::Vec3: {
                auto val = va_arg(args, glm::vec3);
                pushValTypeVec3(itV, val);
            }
                break;
            case ShaderValueType::Vec4: {
                auto val = va_arg(args, glm::vec4);
                pushValTypeVec4(itV, val);
            }
                break;
            default:
                continue;
        }
    }
    va_end(args);
    return *this;
}

void MeshBuilder::triangle(glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3, glm::vec2 uv1, glm::vec2 uv2, glm::vec2 uv3,
                           glm::vec4 color1, glm::vec4 color2, glm::vec4 color3) {
    XGEAssert(mesh->drawMode == xge::Mesh::DrawMode::TRIANGLES);
    auto itV = attributeValues.begin();
    auto itU = attributeUsages.begin();
    vertexCount += 3;
    for (auto it = attributes.begin(); it != attributes.end() && itV != attributeValues.end() &&
            itU != attributeUsages.end(); it++, itV++, itU++) {
        AttributeUsage usage = *itU;
        switch (usage) {
            case AttributeUsage::POSITION:
                if (it->type == ShaderValueType::Vec3) {
                    pushValTypeVec3(itV, pos1);
                    pushValTypeVec3(itV, pos2);
                    pushValTypeVec3(itV, pos3);
                } else if (it->type == ShaderValueType::Vec2) {
                    pushValTypeVec2(itV, pos1);
                    pushValTypeVec2(itV, pos2);
                    pushValTypeVec2(itV, pos3);
                } else {
#ifndef NDEBUG
                    Log::error("MeshBuilder", "Invalid position attribute type");
#endif
                    abort();
                }
                break;
            case AttributeUsage::TEXTURE_UV:
                pushValTypeVec2(itV, uv1);
                pushValTypeVec2(itV, uv2);
                pushValTypeVec2(itV, uv3);
                break;
            case AttributeUsage::COLOR:
                pushValTypeVec4(itV, color1);
                pushValTypeVec4(itV, color2);
                pushValTypeVec4(itV, color3);
                break;
            default: {
#ifndef NDEBUG
                Log::error("MeshBuilder", "AttributeUsage::CUSTOM is not allowed when using MeshBuilder::triangle");
#endif
                abort();
            }
        }
    }
}

void MeshBuilder::quad(glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3, glm::vec3 pos4,
                       glm::vec2 uv1, glm::vec2 uv2, glm::vec2 uv3, glm::vec2 uv4,
                       glm::vec4 color1, glm::vec4 color2, glm::vec4 color3, glm::vec4 color4) {
    triangle(pos3, pos2, pos1, uv3, uv2, uv1, color3, color2, color1);
    triangle(pos3, pos1, pos4, uv3, uv1, uv4, color3, color1, color4);
}

std::shared_ptr<Mesh> MeshBuilder::build(bool reuse) {
    std::shared_ptr<Mesh> ret = std::move(mesh);
    ret->vertexCount = vertexCount;
    auto itV = attributeValues.begin();
    auto itBU = attributeBufferUsages.begin();
    for (auto it = attributes.begin(); it != attributes.end() && itV != attributeValues.end() &&
            itBU != attributeBufferUsages.end(); it++, itV++, itBU++) {
        Mesh::AttributeValue val;
        val.attribute = *it;
        val.buffer.upload(it->type, *itV, *itBU);
        ret->attributes[it->attributeId] = std::move(val);
    }
    if (reuse) {
        mesh = std::shared_ptr<Mesh>(new Mesh());
        mesh->program = ret->program;
        mesh->drawMode = ret->drawMode;
        mesh->uniforms = ret->uniforms;
    } else {
        mesh.reset();
        attributes.clear();
        attributeUsages.clear();
        attributeValues.clear();
        vertexCount = 0;
    }
    return ret;
}