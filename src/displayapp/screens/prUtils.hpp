#pragma once
#include "prInterfaces.hpp"
#include "prFixedStream.hpp"
#include <lvgl/lvgl.h>

#include <type_traits>

#include <cstdint>

#define SVL(sv)     ((int) (sv).size())
#define SVD(sv)     ((sv).data())
#define SV_ARGS(sv) (int) (sv).size(), (sv).data()

namespace pr
{

using Coord = lv_coord_t;

void writeExample(IFilePtr& f);
void writeExample2(IFilePtr& f);
void writeExample3(IFilePtr& f);
constexpr const char* const fileTxt = "/shoppingList.txt";
constexpr const char* const fileDone = "/shoppingList-done.bin";
constexpr const char* const fileId = "/shoppingList-id.bin";
constexpr const char* const fileAmount = "/shoppingList-amount.bin";
constexpr const char* const fileTmp = "/shoppingList.tmp";
constexpr const char* const fileExtTmp = "/shoppingList.ext.tmp";
constexpr const char* const fileExtIn = "/shoppingList.ext.in";
constexpr const char* const fileExtOut = "/shoppingList.ext.out";

constexpr int fileIdLen = 12;
constexpr int maxItems = 128;
constexpr int bitsPerAmount = 3;
constexpr std::size_t u32PerAmounts = (maxItems * bitsPerAmount + 31) / 32;
constexpr int fileAmountLen =
    sizeof(int) + ((maxItems * bitsPerAmount + 7) >> 3);

/**
 * @deprecated
 */
constexpr int rawLen = (maxItems + 7) >> 3;
/**
 * @deprecated
 */
constexpr int doneFileLen = sizeof(int) + (rawLen << 1);

void splitPacket(IFileFactory& factory);
void splitPacket2(IFileFactory& factory);
void splitPacket3(IFileFactory& factory);
bool concatPacket(IFileFactory& factory);
bool concatPacket2(IFileFactory& factory);
bool concatPacket3(IFileFactory& factory);

/**
 * Daten zu einer Vertauschung zweier benachbarter Items.
 * pos: Index des linken der beiden Items in der Einkaufsliste.
 * offsetPos: Seek-Offset in `fileTxt` des linken Eintrags
 * offsetNext: Seek-Offset in `fileTxt` des rechten Eintrags
 * offsetEnd: Seek-Offset der Position nach dem abschließenden '\n' des rechten
 * Eintrags
 *
 * Falls kein Swap durchgeführt werden konnte, kann pos auf -1 gesetzt werden um
 * dies zu signalisieren.
 */
struct SwapData {
    int pos;
    int offsetPos;
    int offsetNext;
    int offsetEnd;
};

void dumpBtnMatCtrl(const char* label, lv_btnmatrix_ctrl_t ctrl);

pr::FixedStream& formatItem(pr::FixedStream& ss, std::string_view item);
#include <cstdint>

// Hilfsfunktion: Konvertiert ein einzelnes Hex-Zeichen in eine Zahl
constexpr uint8_t hexCharToInt(char c)
{
    if (c >= '0' && c <= '9')
        return c - '0';
    if (c >= 'a' && c <= 'f')
        return c - 'a' + 10;
    if (c >= 'A' && c <= 'F')
        return c - 'A' + 10;
    return 0;
}

// Der "Color Constructor" für den VS Code Farbdialog
constexpr lv_color_t color(const char* hexStr)
{
    // Erwartet das Format "#RRGGBB" (Länge 7)
    // Wenn das '#' fehlt, verschieben sich die Indizes um 1
    int offset = (hexStr[0] == '#') ? 1 : 0;

    uint8_t r = (hexCharToInt(hexStr[offset + 0]) << 4) |
                hexCharToInt(hexStr[offset + 1]);
    uint8_t g = (hexCharToInt(hexStr[offset + 2]) << 4) |
                hexCharToInt(hexStr[offset + 3]);
    uint8_t b = (hexCharToInt(hexStr[offset + 4]) << 4) |
                hexCharToInt(hexStr[offset + 5]);

    // LVGL v7 Macro zur Farberzeugung nutzen
    return LV_COLOR_MAKE(r, g, b);
}

class DoubleEndedList;

class BaseListItem
{
  private:
    DoubleEndedList* list;
    BaseListItem* prev;
    BaseListItem* next;

  public:
    BaseListItem();
    const char* invariants() const;

    BaseListItem* getPrev() const
    {
        return prev;
    }

    BaseListItem* getPrev()
    {
        return prev;
    }

    BaseListItem* getNext() const
    {
        return next;
    }

    BaseListItem* getNext()
    {
        return next;
    }

    /**
     * if this item is part of a list, it is first removed from that list to
     * avoid dangling pointers.
     */
    virtual ~BaseListItem();

    friend class DoubleEndedList;
};

class DoubleEndedList
{
    BaseListItem* first;
    BaseListItem* last;

  public:
    DoubleEndedList();
    const char* invariants() const;
    /**
     * If this list still contains items, the items are first removed from this
     * list to avoid dangling pointers.
     */
    ~DoubleEndedList();
    /**
     * If newItem is nullptr, the call is ignored.
     * If newItem is part of a list (this one or another one), it is first
     * removed from that list, before inserted into this list.
     */
    void insertBefore(BaseListItem* newItem, BaseListItem* beforeThat);
    /**
     * If this list does not contain `removeThat`, the call is ignored.
     */
    void remove(BaseListItem* removeThat);

    void clear();

    BaseListItem* getFirst() const
    {
        return first;
    }

    BaseListItem* getFirst()
    {
        return first;
    }

    BaseListItem* getLast() const
    {
        return last;
    }

    BaseListItem* getLast()
    {
        return last;
    }
};

#define COL_CLASSIC "#ffffff "
#define COL_CHECK   "#86ff96 "
#define COL_STEP_BW "#ffb74c "
#define COL_STEP_FW "#f3ff88 "
#define COL_PAGE_BW "#ff6d49 "
#define COL_PAGE_FW "#aabbcc "
#define COL_MV_BW   "#fc38eb "
#define COL_MV_FW   "#6da7ff "

constexpr unsigned int roundedIntDiv(unsigned int dividend,
                                     unsigned int divisor)
{
    return (dividend + (divisor >> 1)) / divisor;
}

// Diese EINE Funktion funktioniert automatisch für JEDES Enum!
template <typename T>
constexpr std::underlying_type_t<T> to_underlying(T e) noexcept
{
    return static_cast<std::underlying_type_t<T>>(e);
}

} // namespace pr
