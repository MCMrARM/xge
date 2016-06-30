#pragma once


#include <cstddef>
#include <cstring>
#include "Log.h"

namespace xge {

    class DataBuffer {

    private:
        char *data = nullptr;
        size_t size = 0;
        bool isSystemLittleEndian;

    public:
        DataBuffer() {
            char d[] = {1, 0};
            isSystemLittleEndian = (*((short *) d) == 1);
        }

        DataBuffer(char *data, size_t size) : DataBuffer() {
            setBuffer(data, size);
        }

        inline void setBuffer(char *data, size_t size) {
            this->data = data;
            this->size = size;
        }

        template <typename T>
        inline static void swapEndian(T &ptr) {
            char *data = (char *) &ptr;
            char inData[sizeof(T)];
            memcpy(inData, data, sizeof(T));
            for (int i = sizeof(T) - 1; i >= 0; i--) {
                data[sizeof(T) - 1 - i] = inData[i];
            }
        }

        /**
         * This function will compare the endianess to platform endianess and swap it if needed.
         */
        template <typename T>
        inline void swapEndianTo(T &ptr, bool toLittleEndian) {
            if (toLittleEndian != isSystemLittleEndian)
                swapEndian<T>(ptr);
        }

        /**
         * This function reads a primitive type from the buffer. Trying to read non-primitive types can lead to errors.
         */
        template <typename T>
        inline T get(size_t off, bool littleEndian = true) {
#ifndef NDEBUG
            if (off < 0 || off + sizeof(T) >= size) {
                Log::error("DataBuffer", "Trying to read outside DataBuffer boundaries.");
                return T();
            }
#endif
            T val = (T &) data[off];
            swapEndianTo(val, littleEndian);
            return val;
        }

        /**
         * This function writes a primitive type to the buffer. Trying to write non-primitive types can lead to errors.
         */
        template <typename T>
        inline void set(size_t off, T val, bool littleEndian = true) {
#ifndef NDEBUG
            if (off < 0 || off + sizeof(T) >= size) {
                Log::error("DataBuffer", "Trying to write outside DataBuffer boundaries.");
                return;
            }
#endif
            swapEndianTo(val, littleEndian);
            (T &) data[off] = val;
        }

    };

}