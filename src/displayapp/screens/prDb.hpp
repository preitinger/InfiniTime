#pragma once

#include "prAmountState.hpp"
#include "prTextWindow.hpp"
#include "prUncheckedIndex.hpp"

#include "displayapp/LittleVgl.h"

#include <string_view>
#include <array>

namespace pr
{

class Db
{
  public:
    using Buf = std::array<char, pr::HALF_SIZE>;
    Buf buf;
    pr::AmountState amountState;
    pr::TextWindow textWindow;
    pr::UncheckedIndex uncheckedIndex;
    /**
     * >= -1, -1 bedeutet in erster Zeile wird Meta-Zeile "  The Begin."
     * angezeigt.
     */
    int listScrollOffset;
    Pinetime::Components::LittleVgl& lvgl;

    Db(IFileFactory& fileFactory, Pinetime::Components::LittleVgl& lvgl);
    Db(const Db&) = delete;
    void init(IFileFactory& fileFactory);
    std::string_view item(int newPos);
    void swap(int newPos);
    /**
     * @return - old amount, or -1 if check did not do anything.
     */
    int check();
    bool stepFw();
    bool stepBw();
    bool mvFw();
    bool mvBw();
};

} // pr
