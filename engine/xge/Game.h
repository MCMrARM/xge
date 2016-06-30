#pragma once

namespace xge {

    class Game {

    private:
        int w = 0, h = 0;

    public:
        virtual void init() { }

        virtual void draw() { }

        /**
         * This function will be called even before init and will initialize the window/screen size.
         */
        virtual void setSize(int w, int h) {
            this->w = w;
            this->h = h;
        }
        inline int getWidth() {
            return w;
        }
        inline int getHeight() {
            return h;
        }

    };

}