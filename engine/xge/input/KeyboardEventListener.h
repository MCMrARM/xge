#pragma once

#include "KeyboardEvent.h"

namespace xge {

    struct KeyboardEventListener {

        virtual void onKeyPress(KeyPressEvent const &event) { }
        virtual void onKeyRelease(KeyReleaseEvent const &event) { }

        virtual void onCharTyped(unsigned int charCode) { }

    };

}