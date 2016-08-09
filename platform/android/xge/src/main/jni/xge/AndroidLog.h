#pragma once

#include <xge/util/Log.h>

namespace xge {

    class AndroidLog : public Log {

    public:
        virtual void print(LogLevel level, const char* tag, const char* text);

    };

}
