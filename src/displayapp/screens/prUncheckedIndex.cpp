#include "prUncheckedIndex.hpp"
#include "prLog.hpp"

#include <algorithm>
#include <utility>
#include <iterator>
#include <cassert>
#include <cstring>

namespace pr {

    UncheckedIndex::UncheckedIndex() : uncheckedPos(), end(uncheckedPos.begin()) {
        assert(end >= uncheckedPos.begin());
    }

    void UncheckedIndex::swap(const SwapData& sd) {
        auto pred = [&sd](const UncheckedPos& a) {
            return !a.end && (a.pos == sd.pos || a.pos == sd.pos + 1);
        };
        auto it = std::find_if(this->uncheckedPos.begin(), this->uncheckedPos.end(), pred);
        if (it == this->uncheckedPos.end())
            return;
        auto& x = *it;
        assert(!x.end);
        // log("x %d", x.pos);
        std::ptrdiff_t lenPos = sd.offsetNext - sd.offsetPos;
        std::ptrdiff_t lenNext = sd.offsetEnd - sd.offsetNext;
        if (x.pos == sd.pos) {
            assert(!x.end);
            ++x.pos;
            x.offsetBegin += lenNext;
            if (++it < end) {
                auto& y = *it;
                if (!y.end && y.pos == sd.pos + 1) {
                    --y.pos;
                    y.offsetBegin -= lenPos;
                    std::swap(x, y);
                    // Nun ist uncheckedPos wieder garantiert sortiert unter der Bedingung, dass es vor dem Methodenaufruf sortiert war.
                }
            }
        } else {
            assert(x.pos == sd.pos + 1); // sonst waere pred falsch gewesen
            --x.pos;
            x.offsetBegin -= lenPos;
        }
        assert(end >= uncheckedPos.begin());
    }

    auto UncheckedPos::operator<(const UncheckedPos& other) const {
        return pos < other.pos;
    }

    void UncheckedIndex::add(const UncheckedPos& x) {
        assert(testSorted());
        auto insertIt = std::lower_bound(uncheckedPos.begin(), end, x);
        if (insertIt != end) {
            if (insertIt->pos == x.pos) {
                assert(insertIt->offsetBegin == x.offsetBegin);
                assert(insertIt->nettoLen == x.nettoLen);
                return; // already included
            }
            std::memmove(insertIt + 1, insertIt, (end - insertIt) * sizeof(UncheckedPos));
        }
        assert(end < uncheckedPos.end());
        *insertIt = x;
        ++end;

        assert(end >= uncheckedPos.begin());
        // Wenn uncheckedPos vor add sortiert war, ist es nun immer noch sortiert.
        assert(testSorted()); // expensive, but optimized out in Release Mode
    }

    UncheckedIndex::Array::const_iterator UncheckedIndex::remove(int pos) {
        // log("==============> UncheckedIndex::remove");
        UncheckedPos dummy {pos, 0, 0};
        auto it = std::lower_bound(uncheckedPos.begin(), end, dummy);
        if (it == end || it->pos != pos) {
            return it;
        }

        assert(end > uncheckedPos.begin());
        std::memmove(it, it + 1, (end - (it + 1)) * sizeof(UncheckedPos));
        --end;
        assert(end >= uncheckedPos.begin());
        return it;
    }

    UncheckedIndex::Array::const_iterator UncheckedIndex::find(int pos) const {
        UncheckedPos dummy {pos, 0, 0};
        auto it = std::lower_bound(uncheckedPos.begin(), static_cast<Array::const_iterator>(end), dummy);
        return it;
    }

    bool UncheckedIndex::testSorted() const {
        assert(end >= uncheckedPos.begin());
        // log("end - uncheckedPos.begin(): %d", static_cast<int>(end - uncheckedPos.begin()));
        if (end < uncheckedPos.begin() + 2)
            return true;
        auto it = uncheckedPos.begin();
        auto next = it;
        ++next;

        for (; next != end; ++it, ++next) {
            if (!(*it < *next))
                return false;
        }

        return true;
    }

    void UncheckedIndex::dump() const {
        log("UncheckedIndex::dump");
        for (auto it = uncheckedPos.begin(); it < end; ++it) {
            log("{.pos=%d, .offsetBegin=%d, .nettoLen=%d}", it->pos, it->offsetBegin, it->nettoLen);
        }
    }

} // namespace pr
