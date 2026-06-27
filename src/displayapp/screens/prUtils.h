#pragma once
#include "prInterfaces.h"

namespace pr {

void writeExample(IFilePtr& f);
constexpr const char* const fileTxt = "/shoppingList.txt";
constexpr const char* const fileDone = "/shoppingList-done.bin";
constexpr const char* const fileTmp = "/shoppingList.tmp";
constexpr const char* const fileExtTmp = "/shoppingList.ext.tmp";
constexpr const char* const fileExtIn = "/shoppingList.ext.in";
constexpr const char* const fileExtOut = "/shoppingList.ext.out";

constexpr int rawLen = (128 + 7) >> 3;
constexpr int doneFileLen = sizeof(int) + (rawLen << 1);

void splitPacket(IFileFactory& factory);
bool concatPacket(IFileFactory& factory);

} // namespace pr
