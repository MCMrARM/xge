#pragma once

class GLFWwindow;

namespace xge {

    class Game;

    class LinuxPlatform {

    private:
        int winW = 640, winH = 480;
        GLFWwindow *window;

    public:
        void startGame(Game &game);

    };

}