#pragma once

#include "prUtils.hpp"

#include <array>

/*
Hier werden die Positionen geskipter Items in /shoppingList.txt festgehalten.
Bei einem Swap wird anhand der übermittelten Daten in einem `SwapData`-Objekt der Index aktualisiert

*/

namespace pr {
struct UncheckedPos {
    /**
     * Position in AmountState
     */
    int pos;
    bool end;
    /**
     * Offset des Beginns in /shoppingList.txt, d.h. TextWindow
     */
    int offsetBegin;
    /**
     * Länge in Bytes ab `offsetBegin` ohne abschließendes `\n`.
     */
    int nettoLen;

    auto operator<(const UncheckedPos& other) const;

};

struct UncheckedWindow {
    int pos;
    int numBefore;
    int numAfter;
};

class UncheckedIndex {
public:
    using Array = std::array<UncheckedPos, maxItems>;

private:
    Array uncheckedPos;
    Array::iterator end;

public:
    UncheckedIndex();
    void swap(const SwapData& sd);
    void add(const UncheckedPos& x);
    Array::const_iterator remove(int pos);
    Array::const_iterator getBegin() const { return uncheckedPos.begin(); }
    Array::const_iterator getEnd() const { return end; }
    Array::const_iterator find(int pos) const;

    bool testSorted() const;
    void dump() const;

};

}
