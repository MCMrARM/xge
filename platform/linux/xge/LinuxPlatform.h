#pragma once

#include <xge/Platform.h>

class GLFWwindow;

namespace xge {

    class Game;

    class LinuxPlatform : public Platform {

    private:
        static LinuxPlatform *instance;

        int winW = 640, winH = 480;
        GLFWwindow *window;

        static void glfwMousePosCallback(GLFWwindow *window, double xpos, double ypos);
        static void glfwMouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
        static void glfwKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);

    public:
        LinuxPlatform();
        ~LinuxPlatform();
        void startGame(Game &game);

    };

}