#include "prFixedStream.hpp"
#include "prLog.hpp"
#include <algorithm>
#include <array>
#include <bit>
#include <charconv>
#include <cstdio>
#include <cassert>

namespace pr
{

FixedStream::FixedStream(char* buf, size_t size)
    : buf(buf), p(buf), end(p + (size - 1))
{
    if (size > 0) {
        *p = *end = 0;
    }
}

void FixedStream::clear()
{
    p = buf;
    if (end >= p) {
        *p = 0;
    }
}

FixedStream& FixedStream::operator<<(char c)
{
    if (p < end) {
        *p++ = c;
        *p = 0;
    }
    return *this;
}

FixedStream& FixedStream::operator<<(std::string_view sv)
{
    if (p < end) {
        auto n = sv.copy(p, end - p);
        // log("<< string_view: n=%d", (int)n);
        p += n;
        *p = 0;
    }

    return *this;
}

struct Conversion {
    std::string_view from, to;
};

FixedStream& FixedStream::appendConverted(std::string_view sv)
{
    std::array conversions {Conversion {"ä", "ae"},
                            Conversion {"ö", "oe"},
                            Conversion {"ü", "ue"},
                            Conversion {"Ä", "Ae"},
                            Conversion {"Ö", "Oe"},
                            Conversion {"Ü", "Ue"}};
    auto it = sv.begin();
    while (p < end && it < sv.end()) {
        bool found = false;
        std::string_view rest(it, sv.end());
        // log("Rest: '%.*s'", (int)rest.length(), rest.data());
        for (auto i = conversions.begin(); i != conversions.end(); ++i) {
            // log("pruefe conversion '%.*s' -> '%.*s'", (int)i->from.length(),
            // i->from.data(), (int)i->to.length(), i->to.data());
            if (std::string_view(it, sv.end()).starts_with(i->from)) {
                *this << i->to;
                it += i->from.length();
                assert(it <= sv.end());
                // log("found");
                found = true;
                break;
            } else {
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

FixedStream& FixedStream::operator<<(int i)
{
    if (p < end) {
        auto result = std::to_chars(p, end, i);
        if (result.ec == std::errc {}) {
            p = result.ptr;
        }
        *p = 0;
    }
    return *this;
}

std::size_t FixedStream::insertLineBreaks(std::size_t maxLineLength,
                                          std::size_t maxLineBreaks,
                                          std::string_view linePrefix)
{
    assert(end - this->buf >= linePrefix.size() + 1);
    std::size_t lineBreaks = 0;
    std::size_t len = 0;
    auto i = this->buf;
    for (; i < p; ++i) {
        // log("i %p, end %p, len %lu", i, end, len);
        if (*i == '\n') {
            if (lineBreaks >= maxLineBreaks) {
                assert(lineBreaks == maxLineBreaks);
                // log("oben: break wegen lineBreaks == maxLineBreaks");
                break;
            }
            ++lineBreaks;
            len = 0;
        } else {
            if (len >= maxLineLength) {
                assert(len == maxLineLength);
                // insert '\n', i.e. move all rest 1 byte to the right and
                // insert '\n'
                if (lineBreaks >= maxLineBreaks) {
                    assert(lineBreaks == maxLineBreaks);
                    // log("unten: break wegen lineBreaks == maxLineBreaks bei *i = %c", *i);
                    break;
                }
                if (end - i < std::ssize(linePrefix) + 1) {
                    // log("break wegen end - i < std::ssize(linePrefix) + 1 bei *i=%c", *i);
                    break;
                }

                // Pruefen, ob ab i noch Platz fuer `linePrefix.size() + 2`
                // Bytes ist. (+1 fuer '\n' und noch mal +1 fuer mind. 1 Zeichen
                // in der neuen Zeile)
                if (end - i < std::ssize(linePrefix) + 2) {
                    // Nicht mehr genug Platz, also einfach abschneiden, sollte
                    // bei ausreichender Puffergröße nie passieren!
                    log("Buffer too small, cutting!");
                    *i = 0;
                    break;
                }
                // std::size_t n = end - (i + linePrefix.size() + 1);
                // log("n %d", (int) n);
                assert(i < end - (linePrefix.size() + 1));
                std::ranges::copy_backward(i,
                                           end - (linePrefix.size() + 1),
                                           end);
                if (p < end - (linePrefix.size() + 1))
                    p += linePrefix.size() + 1;
                else
                    p = end;
                *i = '\n';
                std::ranges::copy(linePrefix, i + 1);
                // std::memmove(i + 1, linePrefix.data(), linePrefix.size());
                i += linePrefix
                         .size() + 1;
                ++lineBreaks;
                len = 0;
                // log("inserted line, lines=%lu", lineBreaks);
                // log("buf bis i incl: '%.*s'", i - buf, buf);
            }
            ++len;
        }
    }
    assert(i <= end);
    *i = 0;
    return lineBreaks;
}

size_t FixedStream::size() const
{
    return p - buf;
}

// void testInsertLineBreaks()
// {
//     log("Starte testInsertLineBreaks...");
//     std::array<char, 16> buf;
//     FixedStream ss(buf.data(), buf.size());
//     ss << "abc";
//     assert(std::string_view(buf.data()) == "abc");
//     [[maybe_unused]] std::size_t numInserted =
//         ss.insertLineBreaks(1, 1, "**");
//     // Vorher: "abc"
//     // Nachher erwartet: "a\n**b"
//     assert(numInserted == 1);
//     int i = 0;
//     for (auto c : buf) {
//         log("%d: %d", i++, (int) c);
//     }

//     log("buf: '%s'", buf.data());
//     assert(std::string_view(buf.data()) == "a\n**b");

//     log("testInsertLineBreaks erfolgreich!");
// }

} // namespace pr
