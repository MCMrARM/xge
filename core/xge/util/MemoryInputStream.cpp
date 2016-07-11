#include "MemoryInputStream.h"
#include <cstring>

using namespace xge;

size_t MemoryInputStream::read(char *data, size_t size) {
    if (size > dataSize - readPos)
        size = dataSize - readPos;
    memcpy(data, &this->data[readPos], size);
    readPos += size;
    return size;
}