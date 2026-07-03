#pragma once

#include "displayapp/Controllers.h"

#include "prInterfaces.hpp"
#include "prLog.hpp"
#include "prUtils.hpp"

#include "nrf_log.h"

#include <string_view>
#include <cassert>
#include <cstring>


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
     * Das erste Zeichen des Textes des Items mit Index pos ist: raw[offsetPos]
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
    template<size_t N>
    SwapData swap(int newPos, std::array<char, N>& buf);
};

// template implementation

template <size_t N>
SwapData TextWindow::swap(int newPos, std::array<char, N>& buf) {
    int code;
    assert(N >= HALF_SIZE);
    if (!file1) return { -1, -1, -1, -1 };
    // LFS_DEBUG("swap newPos %d", newPos);
    // std::string sRaw(snippet(0, rawSize));
    // LFS_DEBUG("raw before swap: '%s'", sRaw.c_str());
    size_t curSizeNetto, nextSizeNetto;
    char* curP;
    char* nextP;
    {
        std::string_view cur(item(newPos));
        if (cur.empty()) {
            log("TextWindow::swap ignored");
            return { -1, -1, -1, -1 };
        }
        curSizeNetto = cur.size();
        assert(curSizeNetto + 1 <= HALF_SIZE / 2);
        memcpy(curP = buf.data(), cur.data(), curSizeNetto);
    }
    {
        std::string_view next(item(newPos + 1));
        if (next.empty()) {
            log("TextWindow::swap ignored");
            return { -1, -1, -1, -1 };
        }
        nextSizeNetto = next.size();
        assert(nextSizeNetto + 1 <= HALF_SIZE / 2);
        memcpy((nextP = buf.data() + curSizeNetto), next.data(), nextSizeNetto);
    }

    // Durch die vorangegangenen Aufrufe von item() und die Tatsache, dass 2 Zeilen garantiert immer kürzer sind als HALF_SIZE, 
    // ist garantiert, dass nun item(pos) und item(pos+1) komplett in raw liegen.
    item(newPos); // um offsetPos wieder auf cur zeigen zu lassen.
    // LFS_DEBUG("offsetPos %d, pos %d", offsetPos, pos);
    auto destp1 = raw.data() + offsetPos;
    memcpy(destp1, nextP, nextSizeNetto);
    destp1[nextSizeNetto] = '\n';
    auto destp2 = raw.data() + offsetPos + nextSizeNetto + 1;
    memcpy(destp2, curP, curSizeNetto);
    assert(destp2[curSizeNetto] == '\n');
    // LFS_DEBUG("destp1 - raw: %d", (int) (destp1 - raw));
    // LFS_DEBUG("destp2 - raw: %d", (int)(destp2 - raw));
    assert(raw[offsetPos + nextSizeNetto] == '\n');
    assert(raw[offsetPos + nextSizeNetto + 1 + curSizeNetto] == '\n');

    file1->seek(this->offsetRaw + offsetPos);
    // code = fs.FileSeek(&file, this->offsetRaw + offsetPos);
    // assert(code >= 0);

    assert(rawSize <= SIZE);
    assert(offsetPos + nextSizeNetto + 1 + curSizeNetto <= rawSize);

    log("Going to write '%.*s' to file at %d",
        //  SVL(snippet(offsetPos, offsetPos + nextSizeNetto + 1 + curSizeNetto)),
        //  SVD(snippet(offsetPos, offsetPos + nextSizeNetto + 1 + curSizeNetto)), 
        SV_ARGS(snippet(offsetPos, offsetPos + nextSizeNetto + 1 + curSizeNetto)),
        offsetRaw + offsetPos);
    code = file1->write(raw.data() + offsetPos, nextSizeNetto + 1 + curSizeNetto);
    // code = fs.FileWrite(&file, raw + offsetPos, next.size() + 1 + cur.size());
    assert(code == (int)nextSizeNetto + 1 + curSizeNetto);
    if (code != (int)nextSizeNetto + 1 + (int)curSizeNetto) {
        NRF_LOG_ERROR("IFile::write did return %d, but not %d", code, (int)nextSizeNetto + 1 + (int)curSizeNetto);
    }


    // sRaw = snippet(0, rawSize);
    // LFS_DEBUG("raw after swap: '%s'", sRaw.c_str());

    return {
        newPos,
        offsetRaw + offsetPos,
        offsetRaw + offsetPos + (int) curSizeNetto + 1,
        offsetRaw + offsetPos + (int) curSizeNetto + 1 + (int) nextSizeNetto + 1,
    };
}

} // namespace pr
