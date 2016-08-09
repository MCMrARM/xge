#pragma once

#include <utility>
#include <EGL/egl.h>

struct ANativeWindow;

namespace xge {

    class EGLHelper {

    private:
        static void createContext();

    public:
        static EGLDisplay display;
        static EGLSurface surface;
        static EGLContext context;
        static EGLConfig config;

        static void createDisplay(ANativeWindow *androidDisplay);
        static void destroyDisplay();

        static void getDisplaySize(int &w, int &h);

        static void swap();

    };

}