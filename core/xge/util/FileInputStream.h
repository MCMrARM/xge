#pragma once

#include <string>
#include "InputStream.h"

namespace xge {

    class FileInputStream : public InputStream {

    private:
        FILE *file;
        bool ownsFile = true;

    public:
        /**
         * This constructor will use the specified FILE* and use it. If you set takeOwnership to true, this class will
         * close the file when destroyed.
         */
        FileInputStream(FILE *file, bool takeOwnership = false) : file(file), ownsFile(takeOwnership) {
            //
        }
        FileInputStream(std::string filename);
        virtual ~FileInputStream();

        virtual size_t read(char *data, size_t size);

    };

}

