#pragma once

#include <xge/Platform.h>
#include "AndroidAssets.h"

class android_app;
class AInputEvent;

namespace xge {

    class AndroidPlatform : public Platform {

    protected:
        static bool loggerInitialized;

        android_app *androidApp;
        bool active = false, gameInitialized = false;
        AndroidAssets androidAssets;
        int prevWidth, prevHeight;
        float dpi;

        static void handleAndroidAppCmd(android_app *app, int cmd);
        static int handleAndroidAppInput(android_app *app, AInputEvent *event);

    public:
        AndroidPlatform(android_app *androidApp);

        static void addAndroidLogger();

        virtual void startGame(Game &game);

    };

}