#pragma once

#include <memory>
#include <set>
#include <vector>
#include <glm/vec2.hpp>
#include <glm/vec3.hpp>
#include <glm/vec4.hpp>
#include "ShaderValue.h"
#include "MeshBuilderConfig.h"

namespace xge {

    class Mesh;

    class MeshEditor {

    protected:
        Mesh &mesh;

        MeshBuilderConfig config;
        std::set<std::pair<unsigned int, std::pair<size_t, unsigned int>>> editEntries; // pair<vertex id, pair<offset in the values vector, count of values>>
        std::vector<ShaderValue> values;

    public:
        MeshEditor(Mesh &mesh, const MeshBuilderConfig &config) : mesh(mesh), config(config) { }

        /**
         * Adds a vertex to the changes list. Argument count must match the count of attributes.
         * The vertex index MUST be smaller than the total vertex count in the Mesh.
         * Avoid editing the same vertex more than once - it'll slow down the code and waste memory.
         */
        MeshEditor &edit(unsigned int i, ...);

        /**
         * Commits the changes to the Mesh.
         */
        void commit();

        MeshEditor &triangle(unsigned int i, glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3,
                             glm::vec2 uv1, glm::vec2 uv2, glm::vec2 uv3,
                             glm::vec4 color1, glm::vec4 color2, glm::vec4 color3);
        inline MeshEditor &triangle(unsigned int i, glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3) {
            return triangle(i, pos1, pos2, pos3, glm::vec2(0.f), glm::vec2(0.f), glm::vec2(0.f),
                            glm::vec4(1.f), glm::vec4(1.f), glm::vec4(1.f));
        }
        inline MeshEditor &triangleUV(unsigned int i, glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3,
                                      glm::vec2 uv1, glm::vec2 uv2, glm::vec2 uv3) {
            return triangle(i, pos1, pos2, pos3, uv1, uv2, uv3, glm::vec4(1.f), glm::vec4(1.f), glm::vec4(1.f));
        }
        inline MeshEditor &triangleColor(unsigned int i, glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3,
                                         glm::vec4 color1, glm::vec4 color2, glm::vec4 color3) {
            return triangle(i, pos1, pos2, pos3, glm::vec2(0.f), glm::vec2(0.f), glm::vec2(0.f), color1, color2, color3);
        }

        // order: bottom left, top left, top right, top bottom
        inline MeshEditor &quad(unsigned int i, glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3, glm::vec3 pos4,
                                glm::vec2 uv1, glm::vec2 uv2, glm::vec2 uv3, glm::vec2 uv4,
                                glm::vec4 color1, glm::vec4 color2, glm::vec4 color3, glm::vec4 color4) {
            triangle(i, pos3, pos2, pos1, uv3, uv2, uv1, color3, color2, color1);
            return triangle(i + 3, pos3, pos1, pos4, uv3, uv1, uv4, color3, color1, color4);
        }
        inline MeshEditor &quad(unsigned int i, glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3, glm::vec3 pos4,
                                glm::vec2 uv1, glm::vec2 uv2, glm::vec2 uv3, glm::vec2 uv4, glm::vec4 color) {
            return quad(i, pos1, pos2, pos3, pos4, uv1, uv2, uv3, uv4, color, color, color, color);
        }
        inline MeshEditor &quad(unsigned int i, glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3, glm::vec3 pos4) {
            return quad(i, pos1, pos2, pos3, pos4, glm::vec2(0.f), glm::vec2(0.f), glm::vec2(0.f), glm::vec2(0.f),
                        glm::vec4(1.f), glm::vec4(1.f), glm::vec4(1.f), glm::vec4(1.f));
        }
        inline MeshEditor &quadUV(unsigned int i, glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3, glm::vec3 pos4,
                                  glm::vec2 uv1, glm::vec2 uv2, glm::vec2 uv3, glm::vec2 uv4) {
            return quad(i, pos1, pos2, pos3, pos4, uv1, uv2, uv3, uv4,
                        glm::vec4(1.f), glm::vec4(1.f), glm::vec4(1.f), glm::vec4(1.f));
        }
        inline MeshEditor &quadColor(unsigned int i, glm::vec3 pos1, glm::vec3 pos2, glm::vec3 pos3, glm::vec3 pos4,
                                     glm::vec4 color1, glm::vec4 color2, glm::vec4 color3, glm::vec4 color4) {
            return quad(i, pos1, pos2, pos3, pos4, glm::vec2(0.f), glm::vec2(0.f), glm::vec2(0.f), glm::vec2(0.f),
                        color1, color2, color3, color4);
        }

        inline MeshEditor &rect(unsigned int i, glm::vec2 pos1, glm::vec2 pos2, glm::vec2 uv1, glm::vec2 uv2,
                                glm::vec4 color1, glm::vec4 color2, glm::vec4 color3, glm::vec4 color4) {
            return quad(i, glm::vec3(pos1.x, pos1.y, 0.f), glm::vec3(pos1.x, pos2.y, 0.f), glm::vec3(pos2.x, pos2.y, 0.f),
                        glm::vec3(pos2.x, pos1.y, 0.f), uv1, glm::vec2(uv1.x, uv2.y), uv2, glm::vec2(uv2.x, uv1.y),
                        color1, color2, color3, color4);
        }
        inline MeshEditor &rect(unsigned int i, glm::vec2 pos1, glm::vec2 pos2, glm::vec2 uv1, glm::vec2 uv2, glm::vec4 color) {
            return rect(i, pos1, pos2, uv1, uv2, color, color, color, color);
        }
        inline MeshEditor &rectUV(unsigned int i, glm::vec2 pos1, glm::vec2 pos2, glm::vec2 uv1, glm::vec2 uv2) {
            return rect(i, pos1, pos2, uv1, uv2, glm::vec4(1.f), glm::vec4(1.f), glm::vec4(1.f), glm::vec4(1.f));
        }
        inline MeshEditor &rectColor(unsigned int i, glm::vec2 pos1, glm::vec2 pos2,
                                     glm::vec4 color1, glm::vec4 color2, glm::vec4 color3, glm::vec4 color4) {
            return rect(i, pos1, pos2, glm::vec2(0.f, 0.f), glm::vec2(0.f, 0.f), color1, color2, color3, color4);
        }
        inline MeshEditor &rectColor(unsigned int i, glm::vec2 pos1, glm::vec2 pos2, glm::vec4 color) {
            return rect(i, pos1, pos2, glm::vec2(0.f, 0.f), glm::vec2(0.f, 0.f), color, color, color, color);
        }

    };

}