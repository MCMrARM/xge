#pragma once

#include <string>
#include <ctime>

namespace xge {

    class Time {

    public:
        /**
         * Returns the current time in milliseconds (since epoch).
         */
        static long long now();

        /**
         * Converts the specified millisecond since epoch time to std::tm.
         */
        static inline std::tm getTimeStruct(long long time);

        /**
         * Converts the specified time (in milliseconds since epoch) to string using the specified format.
         */
        static std::string getString(long long time, std::string format);

        /**
         * Converts the specified time (in milliseconds since epoch) to string using the format %H:%M:%S
         */
        static inline std::string getString(long long time) { return Time::getString(time, "%H:%M:%S"); };

    };

}