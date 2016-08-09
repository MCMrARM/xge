#include "AndroidAssets.h"

#include <xge/util/Log.h>
#include <android/asset_manager.h>

using namespace xge;

AndroidInputStream::~AndroidInputStream() {
    AAsset_close(asset);
    Log::error("AndroidAssets", "CLOSE");
}

size_t AndroidInputStream::read(char *data, size_t size) {
    Log::error("AndroidAssets", "READ %i", (int) size);
    return (size_t) AAsset_read(asset, data, size);
}

std::vector<char> AndroidAssets::readFile(std::string filename) {
    AAsset *asset = AAssetManager_open(assetManager, filename.c_str(), AASSET_MODE_UNKNOWN);
    if (asset == nullptr) {
        Log::error("AndroidAssets", "Trying to read a missing asset: %s", filename.c_str());
        return std::vector<char>();
    }
    std::vector<char> buffer (AAsset_getLength(asset));
    AAsset_read(asset, buffer.data(), buffer.size());
    AAsset_close(asset);
    return buffer;
}

std::unique_ptr<InputStream> AndroidAssets::open(std::string filename) {
    AAsset *asset = AAssetManager_open(assetManager, filename.c_str(), AASSET_MODE_UNKNOWN);
    if (asset == nullptr) {
        Log::error("AndroidAssets", "Trying to read a missing asset: %s", filename.c_str());
        return std::unique_ptr<InputStream>();
    }
    Log::error("AndroidAssets", "OPEN: %s", filename.c_str());
    return std::unique_ptr<InputStream>(new AndroidInputStream(asset));
}