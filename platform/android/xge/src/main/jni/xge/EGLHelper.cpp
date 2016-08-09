#include "EGLHelper.h"

#include <jni.h>
#include <android/asset_manager.h>
#include <android_native_app_glue.h>
#include <xge/util/Log.h>

using namespace xge;

EGLDisplay EGLHelper::display = EGL_NO_DISPLAY;
EGLSurface EGLHelper::surface = EGL_NO_SURFACE;
EGLContext EGLHelper::context = EGL_NO_CONTEXT;
EGLConfig EGLHelper::config = nullptr;

void EGLHelper::createContext() {
    const EGLint contextAttribs[] = {
            EGL_CONTEXT_CLIENT_VERSION, 2,
            EGL_NONE
    };
    context = eglCreateContext(display, config, NULL, contextAttribs);
}

void EGLHelper::createDisplay(ANativeWindow *androidDisplay) {
    if (display == EGL_NO_DISPLAY) {
        const EGLint attribs[] = {
                EGL_RENDERABLE_TYPE, EGL_OPENGL_ES2_BIT,
                EGL_SURFACE_TYPE, EGL_WINDOW_BIT,
                EGL_BLUE_SIZE, 8,
                EGL_GREEN_SIZE, 8,
                EGL_RED_SIZE, 8,
                EGL_DEPTH_SIZE, 16,
                EGL_SAMPLE_BUFFERS, 1,
                EGL_SAMPLES, 4,
                EGL_NONE
        };

        display = eglGetDisplay(EGL_DEFAULT_DISPLAY);

        eglInitialize(display, 0, 0);

        EGLint numConfigs;
        eglChooseConfig(display, attribs, &config, 1, &numConfigs);

        XGEAssert(config != nullptr);

        EGLint format;
        eglGetConfigAttrib(display, config, EGL_NATIVE_VISUAL_ID, &format);
        ANativeWindow_setBuffersGeometry(androidDisplay, 0, 0, format);
    }

    surface = eglCreateWindowSurface(display, config, androidDisplay, NULL);

    if (context == EGL_NO_CONTEXT)
        createContext();
    if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
        int err = eglGetError();
        if (err == EGL_CONTEXT_LOST) {
            Log::trace("EGLHelper", "EGL context lost", err);
            createContext();
            if (eglMakeCurrent(display, surface, surface, context) == EGL_FALSE) {
                Log::error("EGLHelper", "eglMakeCurrent failed [%i]", err);
                abort();
                return;
            }
        } else {
            Log::error("EGLHelper", "eglMakeCurrent failed [%i]", err);
            abort();
            return;
        }
    }

    Log::trace("EGLHelper", "EGL context was successfully obtained");
}

void EGLHelper::destroyDisplay() {
    if (display != EGL_NO_DISPLAY) {
        eglMakeCurrent(display, EGL_NO_SURFACE, EGL_NO_SURFACE, EGL_NO_CONTEXT);
        if (context != EGL_NO_CONTEXT) {
            eglDestroyContext(display, context);
            context = EGL_NO_CONTEXT;
        }
        if (surface != EGL_NO_SURFACE) {
            eglDestroySurface(display, surface);
            surface = EGL_NO_SURFACE;
        }
        eglTerminate(display);
        display = EGL_NO_DISPLAY;
    }
}

void EGLHelper::getDisplaySize(int &w, int &h) {
    eglQuerySurface(display, surface, EGL_WIDTH, &w);
    eglQuerySurface(display, surface, EGL_HEIGHT, &h);
}

void EGLHelper::swap() {
    eglSwapBuffers(display, surface);
}