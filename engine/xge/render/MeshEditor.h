#pragma once

#include <memory>
#include <set>
#include <vector>
#include "ShaderValue.h"

namespace xge {

    class Mesh;

    class MeshEditor {

    protected:
        Mesh &mesh;

        std::set<std::pair<unsigned int, size_t>> editEntries; // pair<vertex id, offset in the values vector>
        std::vector<ShaderValue> values;

    public:
        MeshEditor(Mesh &mesh) : mesh(mesh) { }

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

    };

}