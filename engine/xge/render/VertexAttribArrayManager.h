#pragma once

#include <set>

namespace xge {

    class VertexAttribArrayManager {

    private:
        std::set<int> enabled;

		void disableOpenGL(int id);

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
			for (auto it = enabled.begin(); it != enabled.end(); ) {
				if (v.count(*it) <= 0) {
					disableOpenGL(*it);
					it = enabled.erase(it);
				}
				else {
					it++;
				}
			}
			for (int i : v) {
				if (enabled.count(i) <= 0)
					enable(i);
			}
        }

    };

}