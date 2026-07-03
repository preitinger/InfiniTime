#pragma once
#include "prInterfaces.hpp"
#include "prTextWindow.hpp"
#include "prAmountState.hpp"
#include "displayapp/apps/Apps.h"
#include "displayapp/screens/Screen.h"
#include "displayapp/Controllers.h"
#include "Symbols.h"

namespace Pinetime {
namespace Applications {
namespace Screens {

class ShoppingList2 : public Screen {
public:
    ShoppingList2(Pinetime::Controllers::FS& fs); // Konstruktor
    ~ShoppingList2() override;      // Destruktor (wichtig für LVGL-Cleanup!)
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
    std::array<char, pr::HALF_SIZE> buf;
    pr::AmountState amountState;
    pr::TextWindow textWindow;
    int mode;

    void updateLabels();
};

}
template <>
struct AppTraits<Apps::ShoppingList2> {
    static constexpr Apps app = Apps::ShoppingList2;
    static constexpr const char* icon = Screens::Symbols::list;
    static Screens::Screen* Create(AppControllers& controllers) {
        return new Screens::ShoppingList2(controllers.filesystem);
    }
    static bool IsAvailable(Pinetime::Controllers::FS& filesystem) {
        return true;
    };
};
}
}
