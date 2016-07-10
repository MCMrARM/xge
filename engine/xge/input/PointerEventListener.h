#pragma once

#include "PointerEvent.h"

namespace xge {

    struct PointerEventListener {

        virtual void onPointerPress(PointerPressEvent const &event) { }
        virtual void onPointerRelease(PointerReleaseEvent const &event) { }
        virtual void onPointerMove(PointerMoveEvent const &event) { }

    };

}