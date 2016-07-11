#pragma once

#include "InputStream.h"
#include "Endianness.h"

namespace xge {

    class DataInputStream : public WrapperInputStream {

    protected:
        Endianness endianness;

    public:
        /**
         * This constructor creates a DataInputStream using the specified InputStream as the data source and the
         * specified endianness (or assuming Little Endian if not specified).
         */
        DataInputStream(InputStream &stream, Endianness endianness = Endianness::LITTLE) :
                WrapperInputStream(stream), endianness(endianness) {

        }

        /**
         * This function will set the targeted endianness of this DataInputStream. All writes will be converted to the
         * specified endianness.
         */
        inline void setEndianness(Endianness endianness) {
            this->endianness = endianness;
        }

        template <typename T>
        DataInputStream &operator >>(T &val) {
            read((char *) &val, sizeof(T));
            EndiannessUtil::swapEndianTo(val, endianness);
            return *this;
        }

    };

}