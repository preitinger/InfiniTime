#include "prValidation.hpp"
#include "prLog.hpp"
#include "prUtils.hpp"
#include "prTextWindow.hpp"

namespace pr {

// Requirements:
// Der Inhalt/shoppingList.txt besteht aus Zeilen.
// Keine Zeile ist leer.
// Keine Zeile ist länger als `HALF_SIZE / 2 - 1` Zeichen.
bool validateShoppingListFile(IFileFactory& ff, uint8_t* buf, size_t size) {
    constexpr auto  maxLenNetto = (HALF_SIZE >> 1) - 1;

    auto f = ff.open(fileTxt, IFileFactory::RDONLY);
    if (!f) return false;
    int count = 0;
    int len;

    do {
        len = f->read(buf, size);
        log("len %d", len);
        uint8_t* end = buf + len;

        for (uint8_t* p = buf; p != end; ++p) {
            if (*p == '\n') {
                if (count == 0) {
                    // Leere Zeile
                    log("Leere Zeile in /shoppingList.txt");
                    return false;
                }
                log("Zeile mit Laenge %d - ok", count);
                count = 0;
            }
            else {
                ++count;
                if (count > maxLenNetto) {
                    // Zeile laenger als maxLenNetto Zeichen
                    log("Zeile (ohne \\n) laenger als %d Zeichen in /shoppingList.txt", maxLenNetto);
                    return false;
                }
            }
        }
    } while (len == (int) size);
    return true;
}

} // namespace pr
