#pragma once

#include "prLvglEvent.hpp"
#include "prDb.hpp"
#include "prEvent.hpp"
#include "prRingBuf.hpp"
#include "prBitSeqArray.hpp"
// #include "prStyle.hpp"
#include "prButton.hpp"
#include "prLabel.hpp"
#include "prFixedStream.hpp"

#include "displayapp/widgets/prBaseIndicator.hpp"
#include <lvgl/lvgl.h>

#include <cstdint>

namespace pr
{

class AmountState;
class TextWindow;

class ModListEndEvent
{
  public:
    ModListEndEvent() = default;
};

// class BaseModListEventListener : public BaseListItem
// {
//   public:
//     virtual ~BaseModListEventListener()
//     {
//     }

//     virtual void onModListEnd() = 0;
// };

enum class ModListSubMode {
    CHECK,
    STEP_FW,
    STEP_BW,
    PAGE_FW,
    PAGE_BW,
    MV_FW,
    MV_BW,
};

using Pinetime::Applications::Widgets::BaseIndicator;

class ModList : public EventSource<ModListEndEvent>, public IEventListener<LvglEvent>
{
    struct Done {
        std::uint8_t amount;
    };

    EventSource<LvglEvent>& lvglEventSource;
    std::array<char, 256> text;
    Button button;
    Label label;
    Db* db;
    // DoubleEndedList eventListeners;
    ModListSubMode subMode;
    const char* colCur;
    BaseIndicator baseIndicator;
    std::uint16_t numItems;
    BitSeqArray<3, maxItems> linesPerItem;
    RingBuf<Done, 4> done;

  public:
    ModList(EventSource<LvglEvent>& lvglEventSource);
    void create(lv_obj_t* scr, Db& db, ModListSubMode subMode);
    bool onSwipeRight();
    bool onSwipeDown();
    bool onSwipeUp();
    void update();
    // void addEventListener(BaseModListEventListener* l);
    // void removeEventListener(BaseModListEventListener* l);
    void onEvent(LvglEvent) override;
    // void onShortClicked(Button*) override;
    // void onLongPressed(Button*) override;
    void del();
    ~ModList();

  private:
    /**
     * @param dir - `1` or `-1`
     */
    int itemsPerPage(int dir);
    // void fireEnd();
};

} // namespace pr
