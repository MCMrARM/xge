#pragma once

#include "PointerDevice.h"

#define MAX_MOUSE_BUTTON_COUNT 32

namespace xge {

    class Mouse : public PointerDevice {

    private:
        friend class Platform;

        int x, y;
        bool pressed[MAX_MOUSE_BUTTON_COUNT];

        void setButtonPressed(int button, bool pressed);
        void setPos(int x, int y);

    public:
        Mouse(PointerEventListener &listener);

        virtual PointerDeviceType getType() const {
            return PointerDeviceType::Mouse;
        }

        virtual bool isPressed(int id) const {
            if (id < 0 || id >= MAX_MOUSE_BUTTON_COUNT)
                return false;
            return pressed[id];
        }

        virtual int getX(int pointerId = 0) const {
            return (pointerId == 0 ? x : -1);
        }
        virtual int getY(int pointerId = 0) const {
            return (pointerId == 0 ? y : -1);
        }

    };

}


