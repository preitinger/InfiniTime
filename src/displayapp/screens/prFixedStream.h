#pragma once

#include <string_view>
#include <cstddef>

namespace pr {

class FixedStream {
private:
    char* buf;
    char* p;
    char* end;

public:
    FixedStream(char* buf, size_t size);
    ~FixedStream() = default;
    void clear();
    FixedStream& operator<<(char c);
    FixedStream& operator<<(std::string_view sv);
    FixedStream& operator<<(int i);
    size_t size() const;
    size_t capacity() const { return end >= buf ? end - buf : 0; }

};

}
