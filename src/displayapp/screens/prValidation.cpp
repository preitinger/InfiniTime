#include "prValidation.h"
#include "prLog.h"
#include "prUtils.h"

namespace pr {

// Requirements:
// Der Inhalt/shoppingList.txt besteht aus Zeilen.
// Keine Zeile ist leer.
// Keine Zeile ist länger als 32 Zeichen.
bool validateShoppingListFile(IFileFactory& ff, uint8_t* buf, size_t size) {

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
                if (count > 32) {
                    // Zeile laenger als 32 Zeichen
                    log("Zeile laenger als 32 Zeichen in /shoppingList.txt");
                    return false;
                }
            }
        }
    } while (len == (int) size);
    return true;
}

} // namespace pr
