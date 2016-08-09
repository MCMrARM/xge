#include "AndroidPlatform.h"

#include <android_native_app_glue.h>
#include <xge/Game.h>
#include <xge/opengl.h>
#include "AndroidLog.h"
#include "AndroidAssets.h"
#include "EGLHelper.h"

using namespace xge;

bool AndroidPlatform::loggerInitialized = false;

AndroidPlatform::AndroidPlatform(android_app *androidApp) : androidApp(androidApp),
                                                            androidAssets(androidApp->activity->assetManager) {
    androidApp->userData = this;
    androidApp->onAppCmd = handleAndroidAppCmd;
    androidApp->onInputEvent = handleAndroidAppInput;
    Resources::assets = &androidAssets;
}

void AndroidPlatform::addAndroidLogger() {
    if (loggerInitialized)
        return;
    loggerInitialized = true;
    Log::loggers.push_back(new xge::AndroidLog());
}

void AndroidPlatform::startGame(Game &game) {
    addAndroidLogger();
    gameInstance = &game;
    gameInitialized = false;
    android_poll_source *source;
    int eId, events;
    while (true) {
        while ((eId = ALooper_pollAll(active ? 0 : -1, NULL, &events, (void**) &source)) >= 0) {
            if (source != nullptr)
                source->process(androidApp, source);

            if (androidApp->destroyRequested != 0)
                break;
        }
        if (androidApp->destroyRequested != 0) {
            Log::info("AndroidPlatform", "Destroying...");
            game.suspend();
            EGLHelper::destroyDisplay();
            break;
        }

        if (gameInitialized) {
            int w, h;
            EGLHelper::getDisplaySize(w, h);
            if (w != prevWidth || h != prevHeight) {
                glViewport(0, 0, w, h);
                game.setSize(w, h, dpi);
            }
            game.draw();
            EGLHelper::swap();
        }
    }
    gameInstance = nullptr;
}

void AndroidPlatform::handleAndroidAppCmd(android_app *app, int cmd) {
    AndroidPlatform *platform = (AndroidPlatform *) app->userData;
    switch (cmd) {
        case APP_CMD_INIT_WINDOW:
            Log::info("AndroidPlatform", "Initializing window");
            if (app->window != NULL) {
                AConfiguration *config = AConfiguration_new();
                AConfiguration_fromAssetManager(config, app->activity->assetManager);
                platform->dpi = (float) AConfiguration_getDensity(config);
                AConfiguration_delete(config);
                Log::info("AndroidPlatform", "Display DPI: %f", platform->dpi);
                platform->dpi = 320.f;

                EGLHelper::createDisplay(app->window);
                if (!platform->gameInitialized) {
                    platform->gameInitialized = true;
                    int w, h;
                    EGLHelper::getDisplaySize(w, h);
                    glViewport(0, 0, w, h);
                    platform->prevWidth = w;
                    platform->prevHeight = h;
                    platform->gameInstance->setSize(w, h, platform->dpi);
                    platform->gameInstance->init();
                }
            }
            break;
        case APP_CMD_TERM_WINDOW:
            Log::info("AndroidPlatform", "Terminating window");
            platform->gameInstance->suspend();
            platform->active = false;
            EGLHelper::destroyDisplay();
            break;
        case APP_CMD_RESUME:
            Log::info("AndroidPlatform", "Resuming...");
            platform->active = true;
            break;
        case APP_CMD_PAUSE:
            Log::info("AndroidPlatform", "Pausing...");
            platform->active = false;
            break;
        case APP_CMD_SAVE_STATE:
            app->savedState = NULL;
            app->savedStateSize = 0;
            break;
    }
}

int AndroidPlatform::handleAndroidAppInput(android_app *app, AInputEvent *event) {
    AndroidPlatform *platform = (AndroidPlatform *) app->userData;
    if (AInputEvent_getType(event) == AINPUT_EVENT_TYPE_MOTION) {
        int a = AMotionEvent_getAction(event);
        int action = a & AMOTION_EVENT_ACTION_MASK;

        if (action == AMOTION_EVENT_ACTION_DOWN || action == AMOTION_EVENT_ACTION_POINTER_DOWN ||
            action == AMOTION_EVENT_ACTION_UP || action == AMOTION_EVENT_ACTION_POINTER_UP ||
            action == AMOTION_EVENT_ACTION_CANCEL) {
            int index = (a & AMOTION_EVENT_ACTION_POINTER_INDEX_MASK) >> AMOTION_EVENT_ACTION_POINTER_INDEX_SHIFT;
            int id = AMotionEvent_getPointerId(event, index);
            int x = (int) AMotionEvent_getX(event, index);
            int y = (int) AMotionEvent_getY(event, index);
            platform->updateTouchFinger(id, (action == AMOTION_EVENT_ACTION_DOWN ||
                    action == AMOTION_EVENT_ACTION_POINTER_DOWN), x, y);
        } else if (action == AMOTION_EVENT_ACTION_MOVE) {
            int pCount = AMotionEvent_getPointerCount(event);
            for (int i = 0; i < pCount; i++) {
                int id = AMotionEvent_getPointerId(event, i);
                int x = (int) AMotionEvent_getX(event, i);
                int y = (int) AMotionEvent_getY(event, i);
                platform->updateTouchFinger(id, true, x, y);
            }
        }
        return 1;
    }
    return 0;
}