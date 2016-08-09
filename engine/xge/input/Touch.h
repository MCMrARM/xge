#pragma once

#include "PointerDevice.h"

#define MAX_TOUCH_FINGER_COUNT 10

namespace xge {

    class Touch : public PointerDevice {

    private:
        friend class Platform;

        bool pressed[MAX_TOUCH_FINGER_COUNT];
        int x[MAX_TOUCH_FINGER_COUNT];
        int y[MAX_TOUCH_FINGER_COUNT];

        void updateFinger(int id, bool pressed, int x = -1, int y = -1);

    public:
        Touch(PointerEventListener &listener);

        virtual PointerDeviceType getType() const {
            return PointerDeviceType::Touch;
        }

        virtual bool isPressed(int id) const {
            if (id < 0 || id >= MAX_TOUCH_FINGER_COUNT)
                return false;
            return pressed[id];
        }

        virtual int getX(int pointerId = 0) const {
            if (pointerId < 0 || pointerId >= MAX_TOUCH_FINGER_COUNT)
                return -1;
            return x[pointerId];
        }
        virtual int getY(int pointerId = 0) const {
            if (pointerId < 0 || pointerId >= MAX_TOUCH_FINGER_COUNT)
                return -1;
            return y[pointerId];
        }

    };

}


