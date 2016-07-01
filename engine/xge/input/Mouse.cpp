#include "Mouse.h"
#include "PointerEvent.h"
#include "PointerEventListener.h"

using namespace xge;

Mouse::Mouse(PointerEventListener &listener) : PointerDevice(listener) {
    //
}

void Mouse::setButtonPressed(int button, bool pressed) {
    if (button < 0 || button >= MAX_MOUSE_BUTTON_COUNT || this->pressed[button] == pressed)
        return;
    if (pressed) {
        this->pressed[button] = true;
        listener.onPointerPress(PointerPressEvent (this, x, y));
    } else {
        listener.onPointerRelease(PointerReleaseEvent (this, x, y));
        this->pressed[button] = false;
    }
}

void Mouse::setPos(int x, int y) {
    int dx = x - this->x;
    int dy = y - this->y;
    this->x = x;
    this->y = y;
    listener.onPointerMove(PointerMoveEvent (this, x, y, dx, dy));
}