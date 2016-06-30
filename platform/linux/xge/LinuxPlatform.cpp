#include "LinuxPlatform.h"
#include <xge/opengl.h>
#include <GLFW/glfw3.h>
#include <xge/Game.h>
#include <xge/util/Log.h>
#include <cstdlib>

using namespace xge;

void LinuxPlatform::startGame(Game &game) {
    if (!glfwInit()) {
        Log::error("GLFW", "Call to glfwInit failed");
        return;
    }
    glfwWindowHint(GLFW_SAMPLES, 4);
    window = glfwCreateWindow(640, 480, "Game", NULL, NULL);
    game.setSize(640, 480);
    if (!window) {
        Log::error("GLFW", "Couldn't create window");
        glfwTerminate();
        return;
    }
    glfwMakeContextCurrent(window);
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
    glfwTerminate();
}