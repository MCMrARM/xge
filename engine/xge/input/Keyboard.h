#pragma once

namespace xge {

    class KeyboardEventListener;

    struct ModifierKeysState {
        bool shift;
        bool control;
        bool alt;
        bool super;
    };

    class Keyboard {

    private:
        friend class Platform;

        KeyboardEventListener &listener;
        bool pressed[512];

        void setKeyPressed(int key, bool pressed, ModifierKeysState modifierKeys);

        void onCharTyped(unsigned int charCode);

    public:
        Keyboard(KeyboardEventListener &listener);

        inline bool isPressed(int key) const {
            if (key <= 0 || key >= 512)
                return false;
            return pressed[key];
        }

    };

}
