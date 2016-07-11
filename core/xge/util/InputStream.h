#pragma once

#include <cstddef>

namespace xge {

    class InputStream {

    public:
        virtual ~InputStream() { }
        virtual size_t read(char *data, size_t size) = 0;

    };

    class WrapperInputStream : public InputStream {

    protected:
        InputStream &stream;

    public:
        WrapperInputStream(InputStream &stream) : stream(stream) {
            //
        }

        virtual size_t read(char *data, size_t size) {
            return stream.read(data, size);
        }

    };

}