#include "Resources.h"
#include "Log.h"

#include <fstream>

using namespace xge;

Resources *Resources::assets;
Resources *Resources::userData;

std::vector<char> FileResources::readFile(std::string filename) {
    std::ifstream fs(basePath + "/" + filename, std::ifstream::binary);
    if (!fs) {
        Log::debug("FileResources", "Trying to read a missing file: %s/%s", basePath.c_str(), filename.c_str());
        return std::vector<char>();
    }

    fs.seekg(0, fs.end);
    std::vector<char> data((size_t) fs.tellg());
    fs.seekg(0, fs.beg);
    fs.read(data.data(), data.size());
    if (!fs) {
        Log::debug("FileResources", "Failed to read full file (read %i of %i)", fs.gcount(), data.size());
        data.resize(fs.gcount());
    }
    fs.close();
    return std::move(data);
}