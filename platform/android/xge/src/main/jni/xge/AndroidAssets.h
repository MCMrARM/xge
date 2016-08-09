#pragma once

#include <xge/util/Resources.h>

struct AAsset;
struct AAssetManager;

namespace xge {

    struct AndroidInputStream : public InputStream {

    protected:
        AAsset *asset;

    public:
        AndroidInputStream(AAsset *asset) : asset(asset) { }
        virtual ~AndroidInputStream();

        virtual size_t read(char *data, size_t size);

    };

    class AndroidAssets : public Resources {

    protected:
        AAssetManager *assetManager;

    public:
        AndroidAssets(AAssetManager *assetManager) : assetManager(assetManager) {
            //
        }

        virtual std::vector<char> readFile(std::string filename);
        virtual std::unique_ptr<InputStream> open(std::string filename);

    };

}