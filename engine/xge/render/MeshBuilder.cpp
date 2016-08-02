#include "MeshBuilder.h"

#include <stdarg.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <xge/util/Log.h>

using namespace xge;

#define pushValTypeFloat(vector, val) vector->push_back(AsShaderValue((float) val));
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

#ifdef _WINDOWS
template <typename... T>
MeshBuilder &MeshBuilder::push(T...) {
    vertexCount++;
	auto it = config.attributes.begin();
    auto itV = attributeValues.begin();
	for (auto t : ...) {
        switch (it->type) {
            case ShaderValueType::Float:
                pushValTypeFloat(itV, t);
                break;
            case ShaderValueType::Vec2:
                pushValTypeVec2(itV, t);
                break;
            case ShaderValueType::Vec3:
                pushValTypeVec3(itV, t);
                break;
            case ShaderValueType::Vec4:
                pushValTypeVec4(itV, t);
                break;
            default:
                break;
        }
		it++; itV++;
    }
    return *this;
}
#else
MeshBuilder &MeshBuilder::push(...) {
    va_list args;
    va_start(args, this);
    vertexCount++;
    auto itV = attributeValues.begin();
    for (auto it = config.attributes.begin(); it != config.attributes.end() && itV != attributeValues.end(); it++, itV++) {
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
#endif

MeshBuilder &MeshBuilder::triangle(glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3, glm::vec2 uv1, glm::vec2 uv2, glm::vec2 uv3,
                           glm::vec4 color1, glm::vec4 color2, glm::vec4 color3) {
    XGEAssert(mesh->drawMode == xge::Mesh::DrawMode::TRIANGLES);
    auto itV = attributeValues.begin();
    auto itU = config.attributeUsages.begin();
    vertexCount += 3;
    for (auto it = config.attributes.begin(); it != config.attributes.end() && itV != attributeValues.end() &&
            itU != config.attributeUsages.end(); it++, itV++, itU++) {
        MeshAttributeUsage usage = *itU;
        switch (usage) {
            case MeshAttributeUsage::POSITION:
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
            case MeshAttributeUsage::TEXTURE_UV:
                pushValTypeVec2(itV, uv1);
                pushValTypeVec2(itV, uv2);
                pushValTypeVec2(itV, uv3);
                break;
            case MeshAttributeUsage::COLOR:
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
    return *this;
}

std::shared_ptr<Mesh> MeshBuilder::build(bool reuse) {
    std::shared_ptr<Mesh> ret = std::move(mesh);
    ret->vertexCount = vertexCount;
    auto itV = attributeValues.begin();
    auto itBU = config.attributeBufferUsages.begin();
    for (auto it = config.attributes.begin(); it != config.attributes.end() && itV != attributeValues.end() &&
            itBU != config.attributeBufferUsages.end(); it++, itV++, itBU++) {
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
        config.attributes.clear();
        config.attributeUsages.clear();
        attributeValues.clear();
        vertexCount = 0;
    }
    return ret;
}