#pragma once

#include <set>
#include <memory>
#include <xge/input/Mouse.h>
#include <xge/input/Touch.h>
#include <xge/input/Keyboard.h>
#include <xge/input/PointerEventListener.h>
#include <xge/input/KeyboardEventListener.h>
#include "util/GameTime.h"

namespace xge {

    class Game : public PointerEventListener, public KeyboardEventListener {

    private:
        int w = 0, h = 0;
        float dpi = 0.f;
        Mouse mouse;
        Touch touch;
        std::set<std::shared_ptr<PointerEventListener>> pointerListeners;
        Keyboard keyboard;
        std::set<std::shared_ptr<KeyboardEventListener>> keyboardListeners;
        GameTime time;

    public:
        Game() : mouse(*this), touch(*this), keyboard(*this) {
            //
        }

        virtual void init() { }

        virtual void draw() {
            time.update();
            draw(time);
        }

        virtual void draw(GameTime const &time) { }

        virtual void suspend() { }

        virtual void resume() { }

        /**
         * This function will be called even before init and will initialize the window/screen size.
         */
        virtual void setSize(int w, int h, float dpi) {
            this->w = w;
            this->h = h;
            this->dpi = dpi;
        }
        inline int getWidth() const {
            return w;
        }
        inline int getHeight() const {
            return h;
        }
        inline float getScreenDPI() const {
            return dpi;
        }

        inline Mouse &getMouse() {
            return mouse;
        }
        inline Touch &getTouch() {
            return touch;
        }
        inline Keyboard &getKeyboard() {
            return keyboard;
        }

        inline void addPointerListener(std::shared_ptr<PointerEventListener> listener) {
            pointerListeners.insert(std::move(listener));
        }
        inline void removePointerListener(std::shared_ptr<PointerEventListener> listener) {
            pointerListeners.erase(std::move(listener));
        }
        inline void removeAllPointerListeners() {
            pointerListeners.clear();
        }

        inline void addKeyboardListener(std::shared_ptr<KeyboardEventListener> listener) {
            keyboardListeners.insert(std::move(listener));
        }
        inline void removeKeyboardListener(std::shared_ptr<KeyboardEventListener> listener) {
            keyboardListeners.erase(std::move(listener));
        }
        inline void removeAllKeyboardListeners() {
            keyboardListeners.clear();
        }

        /**
         * Those functions will broadcast the event to the listeners. If you override them, you should call the original
         * function or listeners will not work.
         */
        virtual void onPointerPress(PointerPressEvent const &event) {
            for (auto const &p : pointerListeners)
                p->onPointerPress(event);
        }
        virtual void onPointerRelease(PointerReleaseEvent const &event) {
            for (auto const &p : pointerListeners)
                p->onPointerRelease(event);
        }
        virtual void onPointerMove(PointerMoveEvent const &event) {
            for (auto const &p : pointerListeners)
                p->onPointerMove(event);
        }

        virtual void onKeyPress(KeyPressEvent const &event) {
            for (auto const &k : keyboardListeners)
                k->onKeyPress(event);
        }
        virtual void onKeyRelease(KeyReleaseEvent const &event) {
            for (auto const &k : keyboardListeners)
                k->onKeyRelease(event);
        }
        virtual void onCharTyped(unsigned int charCode) {
            for (auto const &k : keyboardListeners)
                k->onCharTyped(charCode);
        }

    };

}