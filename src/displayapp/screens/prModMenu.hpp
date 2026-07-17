#pragma once

#include "prLvglEvent.hpp"
#include "prEvent.hpp"
#include "prUtils.hpp"
#include "prButton.hpp"
#include "prLabel.hpp"
#include "prModList.hpp"

#include <lvgl/lvgl.h>

namespace pr
{

// class BaseModMenuEventListener : public BaseListItem
// {
//   public:
//     virtual void onModMenuCheck() = 0;
//     virtual void onModMenuStepFw() = 0;
//     virtual void onModMenuStepBw() = 0;
//     virtual void onModMenuPageFw() = 0;
//     virtual void onModMenuPageBw() = 0;
//     virtual void onModMenuMvBw() = 0;
//     virtual void onModMenuMvFw() = 0;
//     virtual void onModMenuEnd() = 0;

//     virtual ~BaseModMenuEventListener()
//     {
//     }
// };

class ModMenu : public EventSource<ModListSubMode>, public IEventListener<LvglEvent>
{
    EventSource<LvglEvent>& lvglEventSource;
    Button bClassic;
    Button bCheck;
    Button bStepBw;
    Button bStepFw;
    Button bPageBw;
    Button bPageFw;
    Button bMvBw;
    Button bMvFw;
    // BaseModMenuEventListener* listener;

  public:
    ModMenu(EventSource<LvglEvent>& lvglEventSource);
    virtual ~ModMenu();
    void create(lv_obj_t* scr, IEventListener<ModListSubMode>* listener);
    void onEvent(LvglEvent) override;
    // void onShortClicked(Button* b) override;

    void del();
};

}