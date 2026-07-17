#include "prDb.hpp"
#include "prModList.hpp"
#include "prUtils.hpp"
#include "prAmountState.hpp"
#include "prTextWindow.hpp"

#include <lvgl/lvgl.h>

namespace pr
{

constexpr int MAX_ROWS = 10;

#define COL_BEGIN "#666666 "
#define COL_END   "#666666 "
#define COL_DONE  "#828d82 "
#define COL_PRESS "#292929"

ModList::ModList(EventSource<LvglEvent>& lvglEventSource)
    : /* buttonStyle(), */
      lvglEventSource(lvglEventSource),
      button(),
      label(),
      db(nullptr),
      //   eventListeners(),
      subMode(ModListSubMode::STEP_FW),
      colCur(""),
      baseIndicator(),
      numItems(0),
      linesPerItem()
{
}

void ModList::create(lv_obj_t* scr, Db& db, ModListSubMode subMode)
{
    lvglEventSource.addListener(this);
    this->db = &db;
    this->numItems = db.textWindow.countItems();
    this->subMode = subMode;
    switch (subMode) {
        case ModListSubMode::CHECK:
            this->colCur = COL_CHECK;
            break;
        case ModListSubMode::STEP_BW:
            this->colCur = COL_STEP_BW;
            break;
        case ModListSubMode::STEP_FW:
            this->colCur = COL_STEP_FW;
            break;
        case ModListSubMode::PAGE_FW:
            this->colCur = COL_PAGE_FW;
            break;
        case ModListSubMode::PAGE_BW:
            this->colCur = COL_PAGE_BW;
            break;
        case ModListSubMode::MV_BW:
            this->colCur = COL_MV_BW;
            break;
        case ModListSubMode::MV_FW:
            this->colCur = COL_MV_FW;
            break;
    }

    button.create(scr,
                  ButtonInit {
                      .x = 0,
                      .y = 0,
                      .w = 240,
                      .h = 240,
                      .eventSource = &lvglEventSource,
                  });
    // button.create(scr, 0, 0, 240, 240, this);
    lv_obj_set_style_local_bg_opa(button.getObj(),
                                  LV_BTN_PART_MAIN,
                                  LV_STATE_DEFAULT,
                                  LV_OPA_COVER);

    // label.create(button, 0, 0, 240, 240);
    // TODO begin test
    label.create(button,
                 LabelInit {
                     .x = 0,
                     .y = 0,
                     .w = 240,
                     .h = 240,
                     .longMode = LongMode::SRoll,
                     .textStatic = this->text.data(),
                     .recolor = true,
                     .bgOpa = LV_OPA_TRANSP,
                 });
    // TODO end test

    lv_obj_set_style_local_bg_opa(label.getObj(),
                                  LV_LABEL_PART_MAIN,
                                  LV_STATE_DEFAULT,
                                  LV_OPA_TRANSP);
    button.setBgColor(Button::Part::MAIN,
                      Button::State::Default,
                      LV_COLOR_MAKE(0, 0, 0));
    button.setBgColor(Button::Part::MAIN,
                      //   Button::State::Pressed,
                      Button::State::Pressed,
                      color(COL_PRESS));
    baseIndicator.Create(0, 1, 0); // wird in update sinnvoll ueberschrieben
    db.listScrollOffset = std::max(db.amountState.pos() - 2, -1);
    update();
}

void ModList::onEvent(LvglEvent e)
{
    switch (e.type) {
        case LvglEventType::Pressed:
            break;
        case LvglEventType::ShortClicked:
            switch (subMode) {
                case ModListSubMode::CHECK: {
                    int oldVal = db->check();
                    if (oldVal != -1) {
                        done.push().amount = static_cast<std::uint8_t>(oldVal);
                        update();
                    }
                    break;
                }
                case ModListSubMode::STEP_FW: {
                    if (db->stepFw()) {
                        update();
                    }
                    break;
                }
                case ModListSubMode::STEP_BW: {
                    if (db->stepBw()) {
                        update();
                    }
                    break;
                }
                case ModListSubMode::PAGE_FW: {
                    db->listScrollOffset =
                        std::max(db->amountState.pos() - 2, -1);
                    int items = itemsPerPage(1);
                    db->amountState.jump(db->amountState.pos() + items);
                    db->listScrollOffset =
                        std::max(db->amountState.pos() - 2, -1);
                    update();
                    break;
                }
                case ModListSubMode::PAGE_BW: {
                    db->listScrollOffset =
                        std::max(db->amountState.pos() - 2, -1);
                    int items = itemsPerPage(-1);
                    db->amountState.jump(db->amountState.pos() - items);
                    db->listScrollOffset =
                        std::max(db->amountState.pos() - 2, -1);
                    update();
                    break;
                }
                case ModListSubMode::MV_FW:
                    if (db->mvFw()) {
                        update();
                    }
                    break;
                case ModListSubMode::MV_BW:
                    if (db->mvBw()) {
                        update();
                    }
                    break;
            }
            break;
        case LvglEventType::LongPressed:
            switch (subMode) {
                case ModListSubMode::CHECK: {
                    if (!done.isEmpty()) {
                        auto amount = done.popBack().amount;
                        db->amountState.prevPos();
                        db->amountState.setAmount(amount);
                        db->listScrollOffset =
                            std::max(db->amountState.pos() - 2, -1);
                        update();
                    }
                    break;
                }
                default:
                    db->amountState.jump(0);
                    db->listScrollOffset = -1;
                    update();
                    break;
            }
            break;
        default:
            break;
    }
}

bool ModList::onSwipeRight()
{
    // log("onSwipeRight");

    if (!this->button.getObj()) {
        log("onSwipeRight return false");
        return false;
    }
    fire(ModListEndEvent {});
    db->lvgl.SetFullRefresh(
        Pinetime::Components::LittleVgl::FullRefreshDirections::Left);
    return true;
}

bool ModList::onSwipeDown()
{
    if (!this->button.getObj()) {
        return true;
    }

    int oldScrollOffset = db->listScrollOffset;
    int items = itemsPerPage(-1);
    assert(items >= 0);
    if (items == 0)
        return true;
    int newScrollOffset = oldScrollOffset - items;
    assert(newScrollOffset >= -1);
    db->listScrollOffset = newScrollOffset;
    update();
    // button.setBgColor(Button::Part::MAIN,
    //                   //   Button::State::Pressed,
    //                   Button::State::Pressed,
    //                   LV_COLOR_MAKE(0, 0, 0));
    db->lvgl.SetFullRefresh(
        Pinetime::Components::LittleVgl::FullRefreshDirections::Down);
    return true;
}

bool ModList::onSwipeUp()
{
    if (!this->button.getObj()) {
        return true;
    }

    int oldScrollOffset = db->listScrollOffset;
    int items = itemsPerPage(1);
    // log("%d items for scrollOffset %d", items, oldScrollOffset);
    assert(items >= 0);
    if (items == 0)
        return true;
    int newScrollOffset = oldScrollOffset + items;
    // log("oldScrollOffset %d, newScrollOffset %d",
    // oldScrollOffset,
    // newScrollOffset);
    db->listScrollOffset = newScrollOffset;
    update();
    // button.setBgColor(Button::Part::MAIN,
    //                   //   Button::State::Pressed,
    //                   Button::State::Pressed,
    //                   LV_COLOR_MAKE(0, 0, 0));
    db->lvgl.SetFullRefresh(
        Pinetime::Components::LittleVgl::FullRefreshDirections::Up);
    return true;
}

void ModList::update()
{
    AmountState& amountState = db->amountState;
    TextWindow& textWindow = db->textWindow;
    int pos = amountState.pos();
    int begin = db->listScrollOffset;
    int end = db->listScrollOffset + MAX_ROWS;

    // log("begin %d, end %d", begin, end);

    char* it = this->text.data();
    std::size_t remainingLineBreaks = 9;
    // bool lastPage = false;
    int itemsOnThisPage = 0;

    for (int i = begin; i < end; ++i) {
        ++itemsOnThisPage;
        if (i < 0) {
            FixedStream ss(it, this->text.size() - (it - this->text.data()));
            ss << COL_BEGIN << "  The Begin.\n";
            --remainingLineBreaks;
            it += ss.size();
        } else {
            auto itemText = textWindow.item(i);
            int amount = amountState.getAmount(i);
            const char* prefix = "";
            {
                FixedStream ss(it,
                               this->text.size() - (it - this->text.data()));
                if (itemText.empty()) {
                    ss << (i == pos ? colCur : COL_END) << "  The End.";
                    // lastPage = true;
                    break;
                }
                if (i == pos) {
                    prefix = colCur;
                    ss << colCur;
                } else if (amount == 0) {
                    prefix = COL_DONE;
                    ss << COL_DONE;
                }
                it += ss.size();
            }
            {
                FixedStream ss(it,
                               this->text.size() - (it - this->text.data()));

                formatItem(ss << amount << "x ", itemText);
                // log("text vor insertLineBreaks: '%s'", it);
                remainingLineBreaks -=
                    ss.insertLineBreaks(20, remainingLineBreaks, prefix);
                // log("text nach insertLineBreaks: '%s'", it);

                if (remainingLineBreaks > 0 && ss.size() < ss.capacity()) {
                    --remainingLineBreaks;
                    ss << '\n';
                    it += ss.size();
                } else {
                    break;
                }
            }
        }
    }

    label.setTextStatic(nullptr); // nullptr fuer Refresh
    // log("itemsOnThisPage %d", itemsOnThisPage);
    baseIndicator.update(begin + 1,
                         itemsOnThisPage,
                         this->numItems + 2 - (begin + 1) - itemsOnThisPage);
}

// void ModList::addEventListener(BaseModListEventListener* l)
// {
//     eventListeners.insertBefore(l, nullptr); // Am Ende hinzufuegen.
// }

// void ModList::removeEventListener(BaseModListEventListener* l)
// {
//     eventListeners.remove(l);
// }

void ModList::del()
{
    clearListeners();
    lvglEventSource.removeListener(this);
    // eventListeners.clear();
    label.del();
    button.del();
}

ModList::~ModList()
{
    del(); // unbedingt notwendig vor Zerstoerung von this->text
}

int ModList::itemsPerPage(int dir)
{
    assert(dir == 1 || dir == -1);
    int firstInPage = db->listScrollOffset;
    int lines = 0;
    int items = 0;
    const int charsPerLine = 20;

    if (dir == 1) {
        while (lines < MAX_ROWS) {
            int pos1 = firstInPage + items;
            if (pos1 < 0) {
                ++items;
                ++lines;
            } else {
                auto item1 = db->item(pos1);
                if (item1.empty()) {
                    // log("item1 empty when firstInPage=%d, items=%d, lines=%d,
                    // "
                    //     "pos1=%d",
                    //     firstInPage,
                    //     items,
                    //     lines,
                    //     pos1);
                    return items;
                }
                FixedStream ss(text.data(), text.size());
                int amount1 = db->amountState.getAmount(pos1);
                formatItem(ss << amount1 << "x ", item1);
                int newLines = (ss.size() + (charsPerLine - 1)) / charsPerLine;
                // log("pos1 %d; newLines: %d", pos1, newLines);
                lines += newLines;
                // log("sum of lines: %d", lines);
                if (lines > MAX_ROWS) {
                    return items;
                }
                ++items;
            }
        }
    } else {
        assert(dir == -1);
        while (lines < MAX_ROWS) {
            int pos1 = firstInPage - 1 - items;
            // log("pos1 %d, items %d, lines %d", pos1, items, lines);
            if (pos1 < -1) {
                break;
            }
            if (pos1 == -1) {
                ++items;
                ++lines;
            } else {
                auto item1 = db->item(pos1);
                if (item1.empty()) {
                    // log("breaking because item empty at pos %d", pos1);
                    break;
                }
                FixedStream ss(text.data(), text.size());
                int amount1 = db->amountState.getAmount(pos1);
                formatItem(ss << amount1 << "x ", item1);
                lines += (ss.size() + (charsPerLine - 1)) / charsPerLine;
                if (lines > MAX_ROWS) {
                    return items;
                }
                ++items;
            }
        }
    }

    return items;
}

#undef COL_PRESS
#undef COL_DONE
#undef COL_END
#undef COL_BEGIN

} // namespace pr