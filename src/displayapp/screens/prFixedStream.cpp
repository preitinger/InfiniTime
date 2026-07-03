#include "prFixedStream.hpp"
#include "prLog.hpp"
#include <algorithm>
#include <array>
#include <charconv>
#include <cstdio>
#include <cstring>
#include <cassert>

namespace pr {

FixedStream::FixedStream(char* buf, size_t size)
    : buf(buf), p(buf), end(p + (size - 1))
{
    if (size > 0) {
        *p = *end = 0;

    }
}
void FixedStream::clear() {
    p = buf;
    if (end >= p) {
        *p = 0;
    }
}
FixedStream& FixedStream::operator<<(char c) {
    if (p < end) {
        *p++ = c;
        *p = 0;
    }
    return *this;
}
FixedStream& FixedStream::operator<<(std::string_view sv) {
    if (p < end) {
        auto n = sv.copy(p, end - p);
        log("<< string_view: n=%d", (int)n);
        p += n;
        *p = 0;
    }

    return *this;
}

struct Conversion {
    std::string_view from, to;
};

FixedStream& FixedStream::appendConverted(std::string_view sv) {
    std::array conversions{
         Conversion{"ä", "ae"},
         Conversion{"ö", "oe"},
         Conversion{"ü", "ue"},
         Conversion{"Ä", "Ae"},
         Conversion{"Ö", "Oe"},
         Conversion{"Ü", "Ue"}
    };
    auto it = sv.begin();
    while (p < end && it < sv.end()) {
        bool found = false;
        std::string_view rest(it, sv.end());
        // log("Rest: '%.*s'", (int)rest.length(), rest.data());
        for (auto i = conversions.begin(); i != conversions.end(); ++i) {
            // log("pruefe conversion '%.*s' -> '%.*s'", (int)i->from.length(), i->from.data(), (int)i->to.length(), i->to.data());
            if (std::string_view(it, sv.end()).starts_with(i->from)) {
                *this << i->to;
                it += i->from.length();
                assert(it <= sv.end());
                // log("found");
                found = true;
                break;
            }
            else {
                // log("not found");
            }
        }
        if (!found) {
            *p++ = *it++;
        }
    }
    assert(p <= end);
    *p = 0;

    return *this;
}

FixedStream& FixedStream::operator<<(int i) {
    if (p < end) {
        auto result = std::to_chars(p, end, i);
        if (result.ec == std::errc{}) {
            p = result.ptr;
        }
        *p = 0;
    }
    return *this;
}
size_t FixedStream::size() const {
    return p - buf;
}


} // namespace pr
