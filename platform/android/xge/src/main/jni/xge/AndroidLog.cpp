#include "AndroidLog.h"

#include <android/log.h>

using namespace xge;

void AndroidLog::print(LogLevel level, const char *tag, const char *text) {
    int androidPriority = ANDROID_LOG_FATAL;
    if (level == LogLevel::LOG_ERROR) androidPriority = ANDROID_LOG_ERROR;
    else if (level == LogLevel::LOG_WARN) androidPriority = ANDROID_LOG_WARN;
    else if (level == LogLevel::LOG_INFO) androidPriority = ANDROID_LOG_INFO;
    else if (level == LogLevel::LOG_DEBUG) androidPriority = ANDROID_LOG_DEBUG;
    else if (level == LogLevel::LOG_TRACE) androidPriority = ANDROID_LOG_VERBOSE;
    __android_log_write(androidPriority, tag, text);
}