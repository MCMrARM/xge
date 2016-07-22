#pragma once

namespace xge {

    class Game;

    class Platform {

    protected:
        Game *gameInstance = nullptr;
        void setMouseButtonPressed(int button, bool pressed);
        void setMousePos(int x, int y);
        void setKeyboardButton(int key, bool pressed,
                               bool shift = false, bool control = false, bool alt = false, bool super = false);
        void charTyped(unsigned int charCode);

    public:
        virtual void startGame(Game &game) = 0;

    };

}


