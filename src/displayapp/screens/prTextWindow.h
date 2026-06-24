#pragma once

#include "displayapp/Controllers.h"

#include "prInterfaces.h"

#include <string_view>


namespace pr {

constexpr int HALF_SIZE = 128;
constexpr int SIZE = HALF_SIZE << 1;


class TextWindow {
private:
    IFilePtr file1;

    std::array<uint8_t, SIZE> raw;
    int rawSize;
    /**
     * Der Offset des ersten Zeichens in raw in der Datei shoppingList.txt
     */
    int offsetRaw;
    /**
     * Index des aktiven Items, oder Anzahl aller Items, falls alle abgearbeitet
     */
    int pos;
    /**
     * Der erste Zeichen des Textes des Items mit Index pos ist: raw[offsetPos]
     */
    int offsetPos;
    bool dirty;

    bool forward();
    bool backward();
    std::string_view snippet(int begin, int end) const;

public:
    TextWindow();
    void init(pr::IFileFactory& fileFactory);
    bool isFileOpen() const { return !!file1; }
    /**
     * Falls dieses Objekt durch einen Aufruf von swap() als dirty markiert wurde,
     * wird der aktuell geladene Inhalt komplett in die Datei geschrieben.
     */
    ~TextWindow();
    std::string_view item(int newPos);
    /**
     * Wenn newPos auf die letzte Zeile zeigt, wird nichts getan.
     * Sonst, wird die aktuelle Zeile newPos mit der folgenden vertauscht und
     * dieses Objekt als dirty markiert.
     */
    void swap(int newPos);
};

} // namespace pr
