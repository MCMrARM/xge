#pragma once

namespace xge {

    class Game;

    class Platform {

    protected:
        Game *gameInstance = nullptr;
        void setMouseButtonPressed(int button, bool pressed);
        void setMousePos(int x, int y);

    public:
        virtual void startGame(Game &game) = 0;

    };

}


