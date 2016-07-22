#pragma once

#include <string>

namespace xge {

    class UTF8String {

    public:
        typedef unsigned int Char;

    private:
        std::string buf;
        size_t len;

    public:
        struct const_iterator {
        private:
            const UTF8String &str;
            size_t off;

            Char getValue();

            unsigned int getValueSize();

        public:
            const_iterator(const UTF8String &str, size_t off) : str(str), off(off) {
            }

            Char operator*() {
                return getValue();
            }
            bool operator==(const_iterator const &i) const {
                return (&i.str == &str && i.off == off);
            }
            bool operator!=(const_iterator const &i) const {
                return (&i.str != &str || i.off != off);
            }

            const_iterator &operator++() {
                off = std::min(off + getValueSize(), str.buf.size());
                return *this;
            }
        };

        UTF8String(const char *str) : buf(str) {
        }
        UTF8String(const std::string &str) : buf(str) {
        }

        const_iterator begin() const { return const_iterator(*this, 0); }

        const_iterator end() const { return const_iterator(*this, buf.size()); }


    };

}