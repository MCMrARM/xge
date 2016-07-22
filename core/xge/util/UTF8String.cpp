#include "UTF8String.h"

using namespace xge;

UTF8String::Char UTF8String::const_iterator::getValue() {
    size_t len = str.buf.length() - off;
    if (len < 1)
        return 0;
    Char c = (Char) str.buf[off];
    if ((c & 0b11000000) == 0b10000000) // invalid
        return 0;

    if ((c & 0b11111110) == 0b11111100) {
        if ((c & 0b1) == 0 || (str.buf[off + 1] & 0b111111) == 0 || (str.buf[off + 2] & 0b111111) == 0 ||
            (str.buf[off + 3] & 0b111111) == 0 || (str.buf[off + 4] & 0b111111) == 0 ||
            (str.buf[off + 5] & 0b111111) == 0)
            return 0;
        return ((((((((((c & 0b1) << 6) | (str.buf[off + 1] & 0b111111)) << 6) | (str.buf[off + 2] & 0b111111)) << 6) |
               (str.buf[off + 3] & 0b111111)) << 6) | (str.buf[off + 4] & 0b111111)) << 6) |
               (str.buf[off + 5] & 0b111111);
    }
    if ((c & 0b11111100) == 0b11111000 && len >= 5) {
        if ((c & 0b11) == 0 || (str.buf[off + 1] & 0b111111) == 0 || (str.buf[off + 2] & 0b111111) == 0 ||
            (str.buf[off + 3] & 0b111111) == 0 || (str.buf[off + 4] & 0b111111) == 0)
            return 0;
        return ((((((((c & 0b11) << 6) | (str.buf[off + 1] & 0b111111)) << 6) | (str.buf[off + 2] & 0b111111)) << 6) |
               (str.buf[off + 3] & 0b111111)) << 6) | (str.buf[off + 4] & 0b111111);
    }
    if ((c & 0b11111000) == 0b11110000 && len >= 4) {
        if ((c & 0b111) == 0 || (str.buf[off + 1] & 0b111111) == 0 || (str.buf[off + 2] & 0b111111) == 0 ||
            (str.buf[off + 3] & 0b111111) == 0)
            return 0;
        return ((((((c & 0b111) << 6) | (str.buf[off + 1] & 0b111111)) << 6) | (str.buf[off + 2] & 0b111111)) << 6) |
               (str.buf[off + 3] & 0b111111);
    }
    if ((c & 0b11110000) == 0b11100000 && len >= 3) {
        if ((c & 0b1111) == 0 || (str.buf[off + 1] & 0b111111) == 0 || (str.buf[off + 2] & 0b111111) == 0)
            return 0;
        return ((((c & 0b1111) << 6) | (str.buf[off + 1] & 0b111111)) << 6) | (str.buf[off + 2] & 0b111111);
    }
    if ((c & 0b11100000) == 0b11000000 && len >= 2) {
        if ((c & 0b11111) == 0 || (str.buf[off + 1] & 0b111111) == 0)
            return 0;
        return ((c & 0b11111) << 6) | (str.buf[off + 1] & 0b111111);
    }
    return c;
}

unsigned int UTF8String::const_iterator::getValueSize() {
    if (str.buf.length() - off < 1)
        return 0;
    Char c = (Char) str.buf[off];
    if ((c & 0b11000000) == 0b10000000) // invalid
        return 1;

    if ((c & 0b11111110) == 0b11111100)
        return 6;
    if ((c & 0b11111100) == 0b11111000)
        return 5;
    if ((c & 0b11111000) == 0b11110000)
        return 4;
    if ((c & 0b11110000) == 0b11100000)
        return 3;
    if ((c & 0b11100000) == 0b11000000)
        return 2;
    return 1;
}