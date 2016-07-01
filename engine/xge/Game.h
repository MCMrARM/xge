#pragma once

#include <set>
#include <memory>
#include <xge/input/Mouse.h>
#include <xge/input/PointerEventListener.h>

namespace xge {

    class Game : public PointerEventListener {

    private:
        int w = 0, h = 0;
        Mouse mouse;
        std::set<std::shared_ptr<PointerEventListener>> pointerListeners;

    public:
        Game() : mouse(*this) {
            //
        }

        virtual void init() { }

        virtual void draw() { }

        /**
         * This function will be called even before init and will initialize the window/screen size.
         */
        virtual void setSize(int w, int h) {
            this->w = w;
            this->h = h;
        }
        inline int getWidth() {
            return w;
        }
        inline int getHeight() {
            return h;
        }

        inline Mouse &getMouse() {
            return mouse;
        }

        inline void addPointerListener(std::unique_ptr<PointerEventListener> listener) {
            pointerListeners.insert(std::move(listener));
        }
        inline void removePointerListener(std::unique_ptr<PointerEventListener> listener) {
            pointerListeners.erase(std::move(listener));
        }
        inline void removeAllPointerListeners() {
            pointerListeners.clear();
        }

        /**
         * Those functions will broadcast the event to the listeners. If you override them, you should call the original
         * function or Pointer Listeners will not work.
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

    };

}