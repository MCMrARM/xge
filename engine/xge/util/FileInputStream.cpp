#include <stdexcept>
#include "FileInputStream.h"

using namespace xge;

FileInputStream::FileInputStream(std::string filename) {
    file = fopen(filename.c_str(), "r");
    if (file == NULL)
        throw std::runtime_error("Failed to open file");
}

FileInputStream::~FileInputStream() {
    if (ownsFile) {
        fclose(file);
    }
}

size_t FileInputStream::read(char *data, size_t size) {
    return fread(data, 1, size, file);
}