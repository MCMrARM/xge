#pragma once

#include "Keyboard.h"

namespace xge {

    struct KeyEvent {

    private:
        Keyboard &keyboard;
        int key;
        ModifierKeysState modifierKeys;

    public:
        KeyEvent(Keyboard &keyboard, int key, ModifierKeysState modifierKeys) : keyboard(keyboard), key(key),
                                                                                modifierKeys(modifierKeys) {
            //
        }

        inline Keyboard &getKeyboard() const {
            return keyboard;
        }

        /**
         * This function returns the id of the key that is directly related to this event.
         */
        inline int getKey() const {
            return key;
        }

        /**
         * This function checks the state of the specified key and returns if it is pressed.
         */
        inline bool isPressed(int key) const {
            return keyboard.isPressed(key);
        }

        inline bool isShiftPressed() {
            return modifierKeys.shift;
        }
        inline bool isControlPressed() {
            return modifierKeys.control;
        }
        inline bool isAltPressed() {
            return modifierKeys.alt;
        }
        inline bool isSuperPressed() {
            return modifierKeys.super;
        }

    };

    struct KeyPressEvent : public KeyEvent {

        KeyPressEvent(Keyboard &keyboard, int key, ModifierKeysState modifierKeys) : KeyEvent(keyboard, key,
                                                                                              modifierKeys) { }

    };

    struct KeyReleaseEvent : public KeyEvent {

        KeyReleaseEvent(Keyboard &keyboard, int key, ModifierKeysState modifierKeys) : KeyEvent(keyboard, key,
                                                                                              modifierKeys) { }

    };


}