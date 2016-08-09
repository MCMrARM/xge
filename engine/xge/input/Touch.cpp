#include "Touch.h"

#include <cstring>
#include "PointerEvent.h"
#include "PointerEventListener.h"

using namespace xge;

Touch::Touch(PointerEventListener &listener) : PointerDevice(listener) {
    memset(pressed, 0, sizeof(pressed));
}

void Touch::updateFinger(int id, bool pressed, int x, int y) {
    if (id < 0 || id >= MAX_TOUCH_FINGER_COUNT)
        return;
    bool wasPressed = this->pressed[id];
    if (pressed && wasPressed) {
        int dx = x - this->x[id];
        int dy = y - this->y[id];
        this->x[id] = x;
        this->y[id] = y;
        listener.onPointerMove(PointerMoveEvent (*this, x, y, dx, dy, id));
    } else if (pressed) {
        this->pressed[id] = true;
        this->x[id] = x;
        this->y[id] = y;
        listener.onPointerPress(PointerPressEvent (*this, x, y, id));
    } else {
        listener.onPointerRelease(PointerReleaseEvent (*this, x, y, id));
        this->pressed[id] = false;
    }
}