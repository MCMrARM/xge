#include "UTF8String.h"
#include "Log.h"

using namespace xge;

unsigned int UTF8String::encodeChar(Char ch, char *out) {
    if (ch > 0b111111111111111111111111111) {
        out[0] = (char) (0b11111100 | ((ch >> 24) & 0b1));
        out[1] = (char) (0b10000000 | ((ch >> 18) & 0b111111));
        out[2] = (char) (0b10000000 | ((ch >> 12) & 0b111111));
        out[3] = (char) (0b10000000 | ((ch >> 6) & 0b111111));
        out[4] = (char) (0b10000000 | (ch & 0b111111));
        return 6;
    }
    if (ch > 0b1111111111111111111111) {
        out[0] = (char) (0b11111000 | ((ch >> 24) & 0b11));
        out[1] = (char) (0b10000000 | ((ch >> 18) & 0b111111));
        out[2] = (char) (0b10000000 | ((ch >> 12) & 0b111111));
        out[3] = (char) (0b10000000 | ((ch >> 6) & 0b111111));
        out[4] = (char) (0b10000000 | (ch & 0b111111));
        return 5;
    }
    if (ch > 0b11111111111111111) {
        out[0] = (char) (0b11110000 | ((ch >> 18) & 0b111));
        out[1] = (char) (0b10000000 | ((ch >> 12) & 0b111111));
        out[2] = (char) (0b10000000 | ((ch >> 6) & 0b111111));
        out[3] = (char) (0b10000000 | (ch & 0b111111));
        return 4;
    }
    if (ch > 0b111111111111) {
        out[0] = (char) (0b11100000 | ((ch >> 12) & 0b1111));
        out[1] = (char) (0b10000000 | ((ch >> 6) & 0b111111));
        out[2] = (char) (0b10000000 | (ch & 0b111111));
        return 3;
    }
    if (ch > 0b1111111) {
        out[0] = (char) (0b11000000 | ((ch >> 6) & 0b11111));
        out[1] = (char) (0b10000000 | (ch & 0b111111));
        return 2;
    }
    out[0] = (char) (ch & 0b1111111);
    return 1;
}

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

unsigned int UTF8String::length() const {
    unsigned int ret = 0;
    for (Char c : *this)
        ret++;
    return ret;
}

UTF8String UTF8String::substr(int start, int len) const {
    XGEAssert(start >= 0 && len >= 0);
    auto it = begin();
    while (start--)
        ++it;
    auto it2 = it;
    while (len--)
        ++it2;
    return UTF8String(buf.substr(it.offset(), it2.offset() - it.offset()));
}