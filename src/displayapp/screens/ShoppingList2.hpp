#pragma once

#include "prLvglEvent.hpp"
#include "prEvent.hpp"
#include "prDb.hpp"
#include "prModMenu.hpp"
#include "prModList.hpp"
#include "prMode2.hpp"
#include "prInterfaces.hpp"
#include "prTextWindow.hpp"
#include "prAmountState.hpp"
#include "prUncheckedIndex.hpp"
#include "displayapp/apps/Apps.h"
#include "displayapp/screens/Screen.h"
#include "displayapp/Controllers.h"
#include "Symbols.h"

namespace Pinetime
{
namespace Applications
{
namespace Screens
{

class ShoppingList2 : public Screen,
                      public pr::IEventListener<pr::ModListEndEvent>,
                      public pr::IEventListener<pr::ModListSubMode>
{
  public:
    ShoppingList2(Pinetime::Controllers::FS& fs, Pinetime::Components::LittleVgl& lvgl); // Konstruktor
    ~ShoppingList2() override; // Destruktor (wichtig für LVGL-Cleanup!)
    void OnButtonEvent(lv_obj_t* obj, lv_event_t event);
    bool OnTouchEvent(TouchEvents event);
    void onEvent(pr::ModListEndEvent) override;
    void onEvent(pr::ModListSubMode) override;
    // void onModMenuCheck() override;
    // void onModMenuStepFw() override;
    // void onModMenuStepBw() override;
    // void onModMenuPageFw() override;
    // void onModMenuPageBw() override;
    // void onModMenuMvBw() override;
    // void onModMenuMvFw() override;
    // void onModMenuEnd() override;

  private:
    // // Hier deklarierst du deine UI-Objekt-Pointer
    Pinetime::Controllers::FS& fs;
    pr::IFileFactoryPtr fileFactory;
    lv_obj_t* postview;
    lv_obj_t* done_btn;
    lv_obj_t* btnRow;
    pr::MyMatrix matBtnRow;
    pr::Db db;
    lv_obj_t* done_label;
    lv_obj_t* preview;
    // int mode;
    bool filter;
    lv_task_t* taskRefresh;
    pr::EventSource<pr::LvglEvent> lvglEventSource;
    pr::ModList modList;
    pr::ModMenu modMenu;

    void updateLabels();
    void Refresh() override;
    void setFilter(bool newFilter);
    // void handleModMenu(pr::ModListSubMode subMode);
    void testModList();
};

}

template <>
struct AppTraits<Apps::ShoppingList2> {
    static constexpr Apps app = Apps::ShoppingList2;
    static constexpr const char* icon = Screens::Symbols::list;

    static Screens::Screen* Create(AppControllers& controllers)
    {
        return new Screens::ShoppingList2(controllers.filesystem, controllers.lvgl);
    }

    static bool IsAvailable(Pinetime::Controllers::FS& filesystem)
    {
        return true;
    };
};
}
}
