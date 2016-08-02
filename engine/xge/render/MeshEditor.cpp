#include "MeshEditor.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <climits>
#include <cstring>
#include <xge/util/Log.h>
#include <xge/util/DynamicStackArray.h>
#include "Mesh.h"
#include "ShaderValueType.h"

#define pushValTypeFloat(vector, val) vector.push_back(AsShaderValue((float) val));
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

using namespace xge;

MeshEditor& MeshEditor::edit(unsigned int i, ...) {
    va_list args;
    va_start(args, i);
    editEntries.insert({i, {values.size(), 1}});
    for (auto it = mesh.attributes.begin(); it != mesh.attributes.end(); it++) {
        switch (it->second.attribute.type) {
            case ShaderValueType::Float:
                pushValTypeFloat(values, (float) va_arg(args, double));
                break;
            case ShaderValueType::Vec2: {
                auto val = va_arg(args, glm::vec2);
                pushValTypeVec2(values, val);
            }
                break;
            case ShaderValueType::Vec3: {
                auto val = va_arg(args, glm::vec3);
                pushValTypeVec3(values, val);
            }
                break;
            case ShaderValueType::Vec4: {
                auto val = va_arg(args, glm::vec4);
                pushValTypeVec4(values, val);
            }
                break;
            default:
                continue;
        }
    }
    va_end(args);
    return *this;
}

MeshEditor& MeshEditor::triangle(unsigned int i, glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3, glm::vec2 uv1,
                                 glm::vec2 uv2, glm::vec2 uv3, glm::vec4 color1, glm::vec4 color2, glm::vec4 color3) {
    XGEAssert(mesh.drawMode == xge::Mesh::DrawMode::TRIANGLES);
    auto itU = config.attributeUsages.begin();
    editEntries.insert({i, {values.size(), 3}});
    for (auto it = config.attributes.begin(); it != config.attributes.end() && itU != config.attributeUsages.end();
         it++, itU++) {
        MeshAttributeUsage usage = *itU;
        switch (usage) {
            case MeshAttributeUsage::POSITION:
                if (it->type == ShaderValueType::Vec3) {
                    pushValTypeVec3(values, pos1);
                    pushValTypeVec3(values, pos2);
                    pushValTypeVec3(values, pos3);
                } else if (it->type == ShaderValueType::Vec2) {
                    pushValTypeVec2(values, pos1);
                    pushValTypeVec2(values, pos2);
                    pushValTypeVec2(values, pos3);
                } else {
#ifndef NDEBUG
                    Log::error("MeshEditor", "Invalid position attribute type");
#endif
                    abort();
                }
                break;
            case MeshAttributeUsage::TEXTURE_UV:
                pushValTypeVec2(values, uv1);
                pushValTypeVec2(values, uv2);
                pushValTypeVec2(values, uv3);
                break;
            case MeshAttributeUsage::COLOR:
                pushValTypeVec4(values, color1);
                pushValTypeVec4(values, color2);
                pushValTypeVec4(values, color3);
                break;
            default: {
#ifndef NDEBUG
                Log::error("MeshEditor", "AttributeUsage::CUSTOM is not allowed when using MeshEditor::triangle");
#endif
                abort();
            }
        }
    }
    return *this;
}

void MeshEditor::commit() {
    size_t attribCount = config.attributes.size();
    StackArray(Mesh::AttributeValue *, attribs, attribCount);
    for (size_t i = 0; i < attribCount; i++)
        attribs[i] = &mesh.attributes.at(config.attributes[i].attributeId);
	StackArray(std::vector<ShaderValue>, attribValues, attribCount);
    unsigned int startVertex = UINT32_MAX;
    unsigned int endVertex = UINT32_MAX;
    for (auto &p : editEntries) {
        unsigned int c = p.second.second;
        if (startVertex == UINT32_MAX) {
            startVertex = p.first;
            endVertex = p.first + c;
        } else if (p.first < endVertex) {
            // we told the user not to do this
            for (size_t i = 0; i < attribCount; i++)
                attribValues[i].resize(attribValues[i].size() - GetShaderValueTypeComponentCount(attribs[i]->attribute.type));
        } else if (endVertex == p.first) {
            endVertex += c;
        } else {
            // id not in order, upload & start new
            for (size_t i = 0; i < attribCount; i++) {
                Mesh::AttributeValue &attrib = *(attribs[i]);
                attrib.buffer.uploadFragment(attrib.attribute.type,
                                             startVertex * GetShaderValueTypeComponentCount(attribs[i]->attribute.type),
                                             attribValues[i]);
                attribValues[i].clear();
            }
            startVertex = p.first;
            endVertex = p.first + c;
        }
        size_t off = p.second.first;
        for (size_t i = 0; i < attribCount; i++) {
            size_t t = p.second.second * GetShaderValueTypeComponentCount(attribs[i]->attribute.type);
            while (t--)
                attribValues[i].push_back(values[off++]);
        }
    }
    if (attribValues[0].size() > 0) {
        for (size_t i = 0; i < attribCount; i++) {
            Mesh::AttributeValue &attrib = *attribs[i];
            attrib.buffer.uploadFragment(attrib.attribute.type,
                                         startVertex * GetShaderValueTypeComponentCount(attribs[i]->attribute.type),
                                         attribValues[i]);
        }
    }
}