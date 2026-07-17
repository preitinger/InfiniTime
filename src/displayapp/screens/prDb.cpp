#include "prDb.hpp"

namespace pr
{

Db::Db(IFileFactory& fileFactory, Pinetime::Components::LittleVgl& lvgl)
    : amountState(fileFactory),
      textWindow(),
      uncheckedIndex(),
      listScrollOffset(-1),
      lvgl(lvgl)
{
}

void Db::init(IFileFactory& fileFactory)
{
    amountState.init();
    textWindow.init(fileFactory);
}

std::string_view Db::item(int pos)
{
    auto res = textWindow.item(pos);

    if (res.empty()) {
        // Ausnahme
        uncheckedIndex.add(pr::UncheckedPos {
            .pos = pos,
            .end = true,
            .offsetBegin = -1,
            .nettoLen = -1,
        });
        return res;
    }

    if (amountState.getAmount(pos) > 0) {
        uncheckedIndex.add(pr::UncheckedPos {
            .pos = pos,
            .end = false,
            .offsetBegin = textWindow.offsetBegin(),
            .nettoLen = static_cast<int>(res.size()),
        });
    } else {
        uncheckedIndex.remove(pos);
    }
    return res;
}

void Db::swap(int newPos)
{
    uncheckedIndex.swap(textWindow.swap(newPos, buf));
}

int Db::check()
{
    int pos = amountState.pos();
    auto item1 = item(pos);
    if (item1.empty()) {
        return -1;
    }
    int oldAmount = static_cast<int>(amountState.getAmount());
    amountState.setAmount(0);
    amountState.nextPos();
    listScrollOffset = std::max(amountState.pos() - 2, -1);
    return oldAmount;
}

bool Db::stepFw()
{
    int pos = amountState.pos();
    std::string_view item1 = item(pos);
    if (item1.empty()) {
        // log("> ignored");
        return false;
    }
    amountState.nextPos();
    listScrollOffset = std::max(amountState.pos() - 2, -1);
    return true;
}

bool Db::stepBw()
{
    if (amountState.pos() > 0) {
        amountState.prevPos();
        listScrollOffset = std::max(amountState.pos() - 2, -1);
        return true;
    } else {
        return false;
    }
}

bool Db::mvFw()
{
    int pos = amountState.pos();
    std::string_view next = item(pos + 1);
    if (next.empty()) {
        return false;
    }
    swap(pos);
    amountState.forward();
    listScrollOffset = std::max(amountState.pos() - 2, -1);
    return true;
}

bool Db::mvBw()
{
    int pos = amountState.pos();
    if (pos < 1 || item(pos).empty()) {
        return false;
    }
    swap(pos - 1);
    amountState.backward();
    listScrollOffset = std::max(amountState.pos() - 2, -1);
    return true;
}

} // pr
