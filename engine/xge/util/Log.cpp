#include "Log.h"
#include <cstdio>
#include <iostream>
#include "Time.h"

using namespace xge;

std::vector<Log *> Log::loggers;

void Log::print(LogLevel level, const char *tag, const char *text) {
    std::cout << Time::getString(Time::now()) << " " << getLogLevelString(level) << ": [" << tag << "] " <<
            text << std::endl;
}

void Log::vlog(LogLevel level, const char *tag, const char *text, va_list args) {
    char buffer[4096];
    vsnprintf(buffer, sizeof(buffer), text, args);

    for (Log *log : loggers)
        log->print(level, tag, buffer);
}