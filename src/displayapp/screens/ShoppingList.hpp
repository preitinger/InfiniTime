#pragma once

#include "prInterfaces.hpp"
#include "prTextWindow.hpp"
#include "prDoneState.hpp"
#include "displayapp/apps/Apps.h"
#include "displayapp/screens/Screen.h"
#include "displayapp/Controllers.h"
#include "Symbols.h"

namespace Pinetime {
namespace Applications {
namespace Screens {

class ShoppingList : public Screen {
public:
    ShoppingList(Pinetime::Controllers::FS& fs); // Konstruktor
    ~ShoppingList() override;      // Destruktor (wichtig für LVGL-Cleanup!)
    void OnButtonEvent(lv_obj_t* obj, lv_event_t event);

private:
    // // Hier deklarierst du deine UI-Objekt-Pointer
    Pinetime::Controllers::FS& fs;
    pr::IFileFactoryPtr fileFactory;
    lv_obj_t* postview;
    lv_obj_t* done_btn;
    lv_obj_t* btnRow;
    lv_obj_t *done_label;
    lv_obj_t* preview;
    std::array<char, 72> buf;
    pr::DoneState doneState;
    pr::TextWindow textWindow;

    void updateLabels();
};

}
template <>
struct AppTraits<Apps::ShoppingList> {
    static constexpr Apps app = Apps::ShoppingList;
    static constexpr const char* icon = Screens::Symbols::list;
    static Screens::Screen* Create(AppControllers& controllers) {
        return new Screens::ShoppingList(controllers.filesystem);
    }
    static bool IsAvailable(Pinetime::Controllers::FS& filesystem) {
        return true;
    };
};
}
}
