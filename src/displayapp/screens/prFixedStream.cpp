#include "prFixedStream.h"
#include <algorithm>
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
        p += n;
        *p = 0;
    }

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
