#include "Platform.h"

#include "Game.h"
#include <xge/util/Log.h>

using namespace xge;

#define CheckGame() \
    if (gameInstance == nullptr) { \
        Log::error("Platform", "Game Instance is null!"); \
        return; \
    }

void Platform::setMousePos(int x, int y) {
    CheckGame()
    gameInstance->getMouse().setPos(x, y);
}

void Platform::setMouseButtonPressed(int button, bool pressed) {
    CheckGame()
    gameInstance->getMouse().setButtonPressed(button, pressed);
}

void Platform::updateTouchFinger(int id, bool pressed, int x, int y) {
    CheckGame()
    gameInstance->getTouch().updateFinger(id, pressed, x, y);
}

void Platform::setKeyboardButton(int key, bool pressed, bool shift, bool control, bool alt, bool super) {
    CheckGame()
    gameInstance->getKeyboard().setKeyPressed(key, pressed, {shift, control, alt, super});
}

void Platform::charTyped(unsigned int charCode) {
    CheckGame()
    gameInstance->getKeyboard().onCharTyped(charCode);
}