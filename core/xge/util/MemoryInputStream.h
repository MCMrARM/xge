#pragma once

#include "InputStream.h"

namespace xge {

    class MemoryInputStream : public InputStream {

    protected:
        char *data;
        size_t dataSize;
        size_t readPos = 0;

    public:
        MemoryInputStream(char *data, size_t size) : data(data), dataSize(size) { }

        virtual size_t read(char *data, size_t size);

    };

}