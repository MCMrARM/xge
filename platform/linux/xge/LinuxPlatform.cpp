#include "LinuxPlatform.h"
#include <xge/opengl.h>
#include <GLFW/glfw3.h>
#include <xge/Game.h>
#include <xge/util/Log.h>
#include <cstdlib>
#ifdef _WINDOWS
#include <WinSock2.h>
#endif

using namespace xge;

LinuxPlatform *LinuxPlatform::instance = nullptr;

LinuxPlatform::LinuxPlatform() {
    if (LinuxPlatform::instance != nullptr) {
        Log::error("LinuxPlatform", "There can be only one LinuxPlatform instance at a time.");
        throw std::logic_error("There can be only one LinuxPlatform instance at a time.");
    }
    LinuxPlatform::instance = this;
}

LinuxPlatform::~LinuxPlatform() {
    LinuxPlatform::instance = nullptr;
}

void LinuxPlatform::startGame(Game &game) {
    if (gameInstance != nullptr) {
        Log::error("LinuxPlatform", "There can be only one game running at a time.");
        throw std::logic_error("There can be only one game running at a time.");
    }
    if (!glfwInit()) {
        Log::error("GLFW", "Call to glfwInit failed");
        return;
    }
#ifdef _WINDOWS
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData)) {
		Log::error("WinSock2", "Failed to initialize WinSock2 library - socket communication will not work");
	}
#endif
    glfwWindowHint(GLFW_SAMPLES, 4);
    glfwSwapInterval(1);
    window = glfwCreateWindow(640, 480, "Game", NULL, NULL);
    game.setSize(640, 480);
    if (!window) {
        Log::error("GLFW", "Couldn't create window");
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
    gameInstance = &game;
    glfwSetWindowSizeCallback(window, LinuxPlatform::glfwWindowSizeCallback);
    glfwSetCursorPosCallback(window, LinuxPlatform::glfwMousePosCallback);
    glfwSetMouseButtonCallback(window, LinuxPlatform::glfwMouseButtonCallback);
    glfwSetKeyCallback(window, LinuxPlatform::glfwKeyCallback);
    glfwSetCharCallback(window, LinuxPlatform::glfwCharCallback);
    glfwSetInputMode(window, GLFW_STICKY_KEYS, 1);
    GLenum err = glewInit();
    if (err != GLEW_OK)
        exit(1);
    glViewport(0, 0, 640, 480);
    game.init();
    while (!glfwWindowShouldClose(window)) {
        game.draw();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    gameInstance = nullptr;
    glfwTerminate();
#ifdef _WINDOWS
	WSACleanup();
#endif
}

void LinuxPlatform::glfwWindowSizeCallback(GLFWwindow *window, int w, int h) {
    glViewport(0, 0, w, h);
    LinuxPlatform::instance->gameInstance->setSize(w, h);
}

void LinuxPlatform::glfwMousePosCallback(GLFWwindow *window, double xpos, double ypos) {
    LinuxPlatform::instance->setMousePos((int) xpos, (int) ypos);
}

void LinuxPlatform::glfwMouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
    if (action == GLFW_PRESS)
        LinuxPlatform::instance->setMouseButtonPressed(button, true);
    else if (action == GLFW_RELEASE)
        LinuxPlatform::instance->setMouseButtonPressed(button, false);
}

void LinuxPlatform::glfwKeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    if (action != GLFW_PRESS && action != GLFW_RELEASE)
        return;
    LinuxPlatform::instance->setKeyboardButton(key, action == GLFW_PRESS, (mods & GLFW_MOD_SHIFT) != 0,
                                               (mods & GLFW_MOD_CONTROL) != 0, (mods & GLFW_MOD_ALT) != 0,
                                               (mods & GLFW_MOD_SUPER) != 0);
}

void LinuxPlatform::glfwCharCallback(GLFWwindow *window, unsigned int charCode) {
    LinuxPlatform::instance->charTyped(charCode);
}