#include "MeshBuilder.h"
#include <stdarg.h>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>

using namespace xge;

#define pushValTypeFloat(vector, val) { \
        ShaderValue shaderValType; \
        shaderValType.f = val; \
        vector->push_back(shaderValType); \
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
                glm::vec2 v2 = va_arg(args, glm::vec2);
                pushValTypeFloat(itV, v2.x);
                pushValTypeFloat(itV, v2.y);
                break;
            }
            case ShaderValueType::Vec3: {
                glm::vec3 v3 = va_arg(args, glm::vec3);
                pushValTypeFloat(itV, v3.x);
                pushValTypeFloat(itV, v3.y);
                pushValTypeFloat(itV, v3.z);
                break;
            }
            case ShaderValueType::Vec4: {
                glm::vec4 v4 = va_arg(args, glm::vec4);
                pushValTypeFloat(itV, v4.x);
                pushValTypeFloat(itV, v4.y);
                pushValTypeFloat(itV, v4.z);
                pushValTypeFloat(itV, v4.w);
                break;
            }
            default:
                continue;
        }
    }
    va_end(args);
    return *this;
}

std::shared_ptr<Mesh> MeshBuilder::build(bool reuse) {
    std::shared_ptr<Mesh> ret = std::move(mesh);
    ret->vertexCount = vertexCount;
    auto itV = attributeValues.begin();
    auto itU = attributeUsages.begin();
    for (auto it = attributes.begin(); it != attributes.end() && itV != attributeValues.end() &&
            itU != attributeUsages.end(); it++, itV++, itU++) {
        Mesh::AttributeValue val;
        val.attribute = *it;
        val.buffer.upload(it->type, *itV, *itU);
        ret->attributes[it->attributeId] = std::move(val);
    }
    if (reuse) {
        mesh = std::shared_ptr<Mesh>(new Mesh());
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