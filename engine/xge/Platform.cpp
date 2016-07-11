#include "Platform.h"

#include "Game.h"
#include <xge/util/Log.h>

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

void Platform::setKeyboardButton(int key, bool pressed, bool shift, bool control, bool alt, bool super) {
    if (gameInstance == nullptr) {
        Log::error("Platform", "Game Instance is null!");
        return;
    }
    gameInstance->getKeyboard().setKeyPressed(key, pressed, {shift, control, alt, super});
}