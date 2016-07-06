#pragma once

#include <vector>
#include <stdarg.h>

#define LogFuncDef(name, logLevel) \
    static void name(const char* tag, const char* text, ...) { \
        va_list args; \
        va_start(args, text); \
        vlog(LogLevel::LOG_TRACE, tag, text, args); \
        va_end(args); \
    }
#ifndef NDEBUG
#define XGEAssert(assert) \
    if (!(assert)) { \
        Log::error("Assert", "Assert failed: %s", #assert); \
        abort(); \
    }
#else
#define XGEAssert(assert)
#endif

namespace xge {

    enum class LogLevel {
        LOG_TRACE,
        LOG_DEBUG,
        LOG_INFO,
        LOG_WARN,
        LOG_ERROR
    };

    class Log {

    protected:

        static inline const char *getLogLevelString(LogLevel lvl) {
            if (lvl == LogLevel::LOG_TRACE) return "Trace";
            if (lvl == LogLevel::LOG_DEBUG) return "Debug";
            if (lvl == LogLevel::LOG_INFO) return "Info";
            if (lvl == LogLevel::LOG_WARN) return "Warn";
            if (lvl == LogLevel::LOG_ERROR) return "Error";
            return "?";
        }

        virtual void print(LogLevel level, const char* tag, const char* text);

        static void vlog(LogLevel level, const char* tag, const char* text, va_list args);

    public:
        static std::vector<Log *> loggers;

        static void addDefaultLogger() {
            loggers.push_back(new Log());
        }

        LogFuncDef(trace, LogLevel::LOG_TRACE)
        LogFuncDef(debug, LogLevel::LOG_DEBUG)
        LogFuncDef(info, LogLevel::LOG_INFO)
        LogFuncDef(warn, LogLevel::LOG_WARN)
        LogFuncDef(error, LogLevel::LOG_ERROR)

    };

}

#undef LogFuncDef