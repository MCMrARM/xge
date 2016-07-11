#pragma once

#include <chrono>

namespace xge {

    class GameTime {

    private:
        typedef std::chrono::steady_clock clock;

        friend class Game;

        clock::time_point start;
        clock::time_point prevFrame;
        clock::time_point currFrame;
        double delta;

        void update() {
            prevFrame = currFrame;
            currFrame = clock::now();
            delta = durationToSeconds(currFrame - prevFrame);
        }

        inline double durationToSeconds(clock::duration d) const {
            return std::chrono::duration_cast<std::chrono::duration<double>>(d).count();
        }

    public:
        GameTime() {
            start = clock::now();
            prevFrame = clock::now();
            currFrame = clock::now();
            delta = 0.f;
        }

        /**
         * Returns the time elapsed since app start, in seconds.
         */
        double getTimeSinceStart() const {
            return durationToSeconds(clock::now() - start);
        }

        /**
         * Returns the delta time between frames, in seconds.
         */
        double getDelta() const {
            return delta;
        }

    };

}

