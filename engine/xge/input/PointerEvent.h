#pragma once

#include "PointerDevice.h"

namespace xge {

    struct PointerEvent {

    private:
        PointerDevice *device;
        int x, y;
        int pointerId = 0;

    public:
        PointerEvent(PointerDevice *device, int x, int y, int pointerId = 0) : device(device), x(x), y(y),
                                                                               pointerId(pointerId) {
            //
        }

        inline PointerDevice *getDevice() const {
            return device;
        }

        inline int getX() const {
            return x;
        }
        inline int getY() const {
            return y;
        }

        /**
         * This will return if the specified touch pointer or mouse button is pressed. When using -1 it'll return if the
         * specified mouse button is pressed or always return true if used with a touch device.
         */
        inline bool isPressed(int id = -1) const {
            return device->isPressed(id == -1 ? pointerId : id);
        }
        /**
         * This function will return x coordinate for the specified pointer id. You should not use any pointer ids
         * different from 0 when using Mouse device as it'll return -1.
         */
        inline int getX(int pointerId) const {
            return device->getX(pointerId);
        }
        /**
         * This function will return y coordinate for the specified pointer id. See getX for details.
         */
        inline int getY(int pointerId) const {
            return device->getY(pointerId);
        }

        inline PointerDeviceType getDeviceType() const {
            return device->getType();
        }
        inline bool isMouse() const {
            return (getDeviceType() == PointerDeviceType::Mouse);
        }
        inline bool isTouch() const {
            return (getDeviceType() == PointerDeviceType::Touch);
        }

        inline int getTouchPointerId() const {
            return pointerId;
        }
        inline int getMouseButtonId() const {
            return pointerId;
        }

    };

    struct PointerPressEvent : public PointerEvent {

        PointerPressEvent(PointerDevice *device, int x, int y, int pointerId = 0) :
                PointerEvent(device, x, y, pointerId) {
            //
        }

    };

    struct PointerReleaseEvent : public PointerEvent {

        PointerReleaseEvent(PointerDevice *device, int x, int y, int pointerId = 0) :
                PointerEvent(device, x, y, pointerId) {
            //
        }

    };

    struct PointerMoveEvent : public PointerEvent {

    private:
        int dx, dy;

    public:
        PointerMoveEvent(PointerDevice *device, int x, int y, int dx, int dy, int pointerId = 0) :
                PointerEvent(device, x, y, pointerId), dx(dx), dy(dy) {
            //
        }

        // Those function returns the delta movement for the current moved pointer.
        inline int getDX() const {
            return dx;
        }
        inline int getDY() const {
            return dy;
        }

    };

}