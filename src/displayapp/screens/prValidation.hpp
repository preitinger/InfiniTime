#pragma once
#include "prInterfaces.hpp"
#include "prFixedStream.hpp"

namespace pr {

// Requirements:
// Der Inhalt/shoppingList.txt besteht aus Zeilen.
// Keine Zeile ist leer.
// Keine Zeile ist länger als 
extern bool validateShoppingListFile(IFileFactory& ff, uint8_t* buf, size_t size);

} // namespace pr
