#include "Keyboard.h"

#include <cstring>
#include "KeyboardEventListener.h"

using namespace xge;

Keyboard::Keyboard(KeyboardEventListener &listener) : listener(listener) {
    memset(pressed, 0, sizeof(pressed));
}

void Keyboard::setKeyPressed(int key, bool pressed, ModifierKeysState state) {
    if (key <= 0 || key >= 512)
        return;
    this->pressed[key] = pressed;
    if (pressed) {
        listener.onKeyPress(KeyPressEvent (*this, key, state));
    } else {
        listener.onKeyRelease(KeyReleaseEvent (*this, key, state));
    }
}