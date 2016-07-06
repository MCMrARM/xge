#pragma once

#include <string>
#include <vector>
#include <memory>
#include "InputStream.h"

namespace xge {

    class Resources {

    public:

        static Resources *assets;
        static Resources *userData;

        virtual std::vector<char> readFile(std::string filename) = 0;
        virtual std::unique_ptr<InputStream> open(std::string filename) = 0;

    };

    class FileResources : public Resources {

    protected:
        std::string basePath;

    public:
        FileResources(std::string basePath) : basePath(basePath) {
            //
        }

        virtual std::vector<char> readFile(std::string filename);
        virtual std::unique_ptr<InputStream> open(std::string filename);

    };

}