#include "Platform.h"

#include "Game.h"
#include "util/Log.h"

using namespace xge;

void Platform::setMousePos(int x, int y) {
    if (gameInstance == nullptr) {
        Log::error("Platform", "Game Instance is null!");
        return;
    }
    gameInstance->getMouse().setPos(x, y);
}

void Platform::setMouseButtonPressed(int button, bool pressed) {
    if (gameInstance == nullptr) {
        Log::error("Platform", "Game Instance is null!");
        return;
    }
    gameInstance->getMouse().setButtonPressed(button, pressed);
}