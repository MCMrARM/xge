#pragma once


#include <cstddef>
#include <cstring>
#include "Log.h"
#include "Endianness.h"

namespace xge {

    class DataBuffer {

    private:
        char *data = nullptr;
        size_t size = 0;
        bool isSystemLittleEndian;

    public:
        DataBuffer() {
        }

        DataBuffer(char *data, size_t size) : DataBuffer() {
            setBuffer(data, size);
        }

        inline void setBuffer(char *data, size_t size) {
            this->data = data;
            this->size = size;
        }

        /**
         * This function reads a primitive type from the buffer. Trying to read non-primitive types can lead to errors.
         */
        template <typename T>
        inline T get(size_t off, Endianness endianness = Endianness::LITTLE) {
#ifndef NDEBUG
            if (off < 0 || off + sizeof(T) >= size) {
                Log::error("DataBuffer", "Trying to read outside DataBuffer boundaries.");
                return T();
            }
#endif
            T val = (T &) data[off];
            EndiannessUtil::swapEndianTo(val, endianness);
            return val;
        }

        /**
         * This function writes a primitive type to the buffer. Trying to write non-primitive types can lead to errors.
         */
        template <typename T>
        inline void set(size_t off, T val, Endianness endianness = Endianness::LITTLE) {
#ifndef NDEBUG
            if (off < 0 || off + sizeof(T) >= size) {
                Log::error("DataBuffer", "Trying to write outside DataBuffer boundaries.");
                return;
            }
#endif
            EndiannessUtil::swapEndianTo(val, endianness);
            (T &) data[off] = val;
        }

    };

}