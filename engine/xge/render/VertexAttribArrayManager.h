#pragma once

#include <set>

namespace xge {

    class VertexAttribArrayManager {

    private:
        std::set<int> enabled;

    public:
        static VertexAttribArrayManager instance;

        void enable(int id);
        void disable(int id);

        inline bool isEnabled(int id) {
            return enabled.count(id) > 0;
        }
        inline std::set<int> const &getEnabled() {
            return enabled;
        }

        inline void set(std::set<int> const &v) {
            for (int i : enabled) {
                if (v.count(i) <= 0)
                    disable(i);
            }
            for (int i : v) {
                if (enabled.count(i) <= 0)
                    enable(i);
            }
        }

    };

}