#include "MeshEditor.h"

#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include <climits>
#include <cstring>
#include <xge/util/Log.h>
#include "Mesh.h"
#include "ShaderValueType.h"

using namespace xge;

MeshEditor& MeshEditor::edit(unsigned int i, ...) {
    va_list args;
    va_start(args, i);
    editEntries.insert({i, values.size()});
    for (auto it = mesh.attributes.begin(); it != mesh.attributes.end(); it++) {
        switch (it->second.attribute.type) {
            case ShaderValueType::Float:
                values.push_back(AsShaderValue((float) va_arg(args, double)));
                break;
            case ShaderValueType::Vec2: {
                auto val = va_arg(args, glm::vec2);
                values.push_back(AsShaderValue(val.x));
                values.push_back(AsShaderValue(val.y));
            }
                break;
            case ShaderValueType::Vec3: {
                auto val = va_arg(args, glm::vec3);
                values.push_back(AsShaderValue(val.x));
                values.push_back(AsShaderValue(val.y));
                values.push_back(AsShaderValue(val.z));
            }
                break;
            case ShaderValueType::Vec4: {
                auto val = va_arg(args, glm::vec4);
                values.push_back(AsShaderValue(val.x));
                values.push_back(AsShaderValue(val.y));
                values.push_back(AsShaderValue(val.z));
                values.push_back(AsShaderValue(val.w));
            }
                break;
            default:
                continue;
        }
    }
    va_end(args);
    return *this;
}

void MeshEditor::commit() {
    size_t attribCount = mesh.attributesOrder.size();
    Mesh::AttributeValue *attribs [attribCount];
    for (size_t i = 0; i < attribCount; i++)
        attribs[i] = &mesh.attributes.at(mesh.attributesOrder[i]);
    std::vector<ShaderValue> attribValues[attribCount];
    unsigned int startVertex = UINT32_MAX;
    unsigned int endVertex = UINT32_MAX;
    size_t prevSizes[attribCount];
    memset(prevSizes, 0, sizeof(prevSizes));
    for (auto &p : editEntries) {
        if (startVertex == UINT32_MAX) {
            startVertex = p.first;
            endVertex = p.first;
        } else if (endVertex == p.first) {
            // we told the user not to do this
            for (unsigned int i = 0; i < attribCount; i++)
                attribValues[i].resize(prevSizes[i]);
        } else if (endVertex == p.first + 1) {
            endVertex++;
        } else {
            // id not in order, upload & start new
            for (size_t i = 0; i < attribCount; i++) {
                Mesh::AttributeValue &attrib = *attribs[i];
                attrib.buffer.uploadFragment(attrib.attribute.type, startVertex, attribValues[i]);
                attribValues[i].clear();
            }
            memset(prevSizes, 0, sizeof(prevSizes));
        }
        for (unsigned int i = 0; i < attribCount; i++)
            prevSizes[i] = attribValues[i].size();
        size_t off = p.second;
        for (size_t i = 0; i < attribCount; i++) {
            switch (attribs[i]->attribute.type) {
                case ShaderValueType::Float:
                    attribValues[i].push_back(values[off++]);
                    break;
                case ShaderValueType::Vec2:
                    attribValues[i].push_back(values[off++]);
                    attribValues[i].push_back(values[off++]);
                    break;
                case ShaderValueType::Vec3:
                    attribValues[i].push_back(values[off++]);
                    attribValues[i].push_back(values[off++]);
                    attribValues[i].push_back(values[off++]);
                    break;
                case ShaderValueType::Vec4:
                    attribValues[i].push_back(values[off++]);
                    attribValues[i].push_back(values[off++]);
                    attribValues[i].push_back(values[off++]);
                    attribValues[i].push_back(values[off++]);
                    break;
                default:
                    continue;
            }
        }
    }
    if (attribValues[0].size() > 0) {
        for (size_t i = 0; i < attribCount; i++) {
            Mesh::AttributeValue &attrib = *attribs[i];
            attrib.buffer.uploadFragment(attrib.attribute.type, startVertex, attribValues[i]);
        }
    }
}