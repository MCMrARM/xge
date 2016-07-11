#pragma once

#include <cstring>

namespace xge {

    enum class Endianness {
        LITTLE, BIG
    };

    class EndiannessUtil {

    public:
        static inline Endianness getSystemEndianness() {
            char d[] = {1, 0};
            return (*((short *) d) == 1 ? Endianness::LITTLE : Endianness::BIG);
        }

        /**
         * This will swap the specified buffer between the little and big endian.
         */
        template <typename T>
        static inline void swapEndian(T &ptr) {
            char *data = (char *) &ptr;
            char inData[sizeof(T)];
            memcpy(inData, data, sizeof(T));
            for (int i = sizeof(T) - 1; i >= 0; i--) {
                data[sizeof(T) - 1 - i] = inData[i];
            }
        }

        /**
         * This function will compare the endianness to platform endianness and swap it if needed.
         */
        template <typename T>
        static inline void swapEndianTo(T &ptr, Endianness toEndianness) {
            if (toEndianness != getSystemEndianness())
                swapEndian<T>(ptr);
        }

    };

}

