#pragma once

#include <string>
#include <algorithm>

namespace xge {

    class UTF8String {

    public:
        typedef unsigned int Char;

        static unsigned int encodeChar(Char ch, char out[]);

    private:
        std::string buf;

    public:
        struct const_iterator {
        protected:
            const UTF8String &str;
            size_t off;

            Char getValue();

            unsigned int getValueSize();

        public:
            const_iterator(const UTF8String &str, size_t off) : str(str), off(off) {
            }

            size_t offset() {
                return off;
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
                off = std::min<size_t>(off + getValueSize(), str.buf.size());
                return *this;
            }
        };

        UTF8String() { }
        UTF8String(const char *str) : buf(str) { }
        UTF8String(const std::string &str) : buf(str) { }
        UTF8String(const UTF8String &str) : buf(str.buf) { }

        unsigned int length() const;

        UTF8String substr(int start, int len = -1) const;

        inline std::string &cpp_str() { return buf; }
        inline const std::string &cpp_str() const { return buf; }

        inline const char *c_str() const { return buf.c_str(); }

        const_iterator begin() const { return const_iterator(*this, 0); }

        const_iterator end() const { return const_iterator(*this, buf.size()); }

        UTF8String operator+(Char ch) const {
            std::string str = buf;
            char data[6];
            unsigned int len = encodeChar(ch, data);
            str.append(data, len);
            return UTF8String (str);
        }
        UTF8String operator+(UTF8String str) const {
            return UTF8String (buf + str.buf);
        }


    };

}