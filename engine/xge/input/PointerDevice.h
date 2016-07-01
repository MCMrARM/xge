#pragma once

namespace xge {

    enum class PointerDeviceType {
        Mouse, Touch
    };

    struct PointerEventListener;

    class PointerDevice {

    protected:
        PointerEventListener &listener;

    public:
        PointerDevice(PointerEventListener &listener) : listener(listener) {
            //
        }

        virtual PointerDeviceType getType() const = 0;

        /**
         * This function returns if the specified button (on mouse) or pointer (on touch) is pressed.
         */
        virtual bool isPressed(int id) const = 0;

        /**
         * Those functions return the position of the specified pointer. If the device only has a single pointer
         * (eg. Mouse) this will only return a valid value when used with pointer id 0. If the pointer doesn't exist
         * it'll return -1.
         */
        virtual int getX(int pointerId = 0) const = 0;
        virtual int getY(int pointerId = 0) const = 0;

    };

}