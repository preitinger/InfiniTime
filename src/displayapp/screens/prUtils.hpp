#pragma once
#include "prInterfaces.hpp"

#define SVL(sv) ((int) (sv).size())
#define SVD(sv) ((sv).data())
#define SV_ARGS(sv) (int) (sv).size(), (sv).data()

namespace pr {

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

constexpr int idLen = 12;
constexpr int rawLen = (128 + 7) >> 3;
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
 * offsetEnd: Seek-Offset der Position nach dem abschließenden '\n' des rechten Eintrags
 * 
 * Falls kein Swap durchgeführt werden konnte, kann pos auf -1 gesetzt werden um dies zu signalisieren.
 */
struct SwapData {
    int pos;
    int offsetPos;
    int offsetNext;
    int offsetEnd;
};

} // namespace pr
