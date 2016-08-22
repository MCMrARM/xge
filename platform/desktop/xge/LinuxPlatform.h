#pragma once

#include <xge/Platform.h>
#include <xge/util/Resources.h>

struct GLFWwindow;
struct GLFWmonitor;

namespace xge {

    class Game;

    class LinuxPlatform : public Platform {

    private:
        static LinuxPlatform *instance;
        FileResources assetResources, dataResources;

        int winW = 640, winH = 480;
        GLFWwindow *window;

        static float getMonitorDPI(GLFWmonitor *monitor);

        static void glfwWindowSizeCallback(GLFWwindow *window, int w, int h);
        static void glfwMousePosCallback(GLFWwindow *window, double xpos, double ypos);
        static void glfwMouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
        static void glfwKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
        static void glfwCharCallback(GLFWwindow *window, unsigned int charCode);

    public:
        LinuxPlatform();
        ~LinuxPlatform();
        void startGame(Game &game);

    };

}