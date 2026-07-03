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

class ShoppingListAdmin : public Screen {
public:
    ShoppingListAdmin(Pinetime::Controllers::FS& fs); // Konstruktor
    ~ShoppingListAdmin() override;      // Destruktor (wichtig für LVGL-Cleanup!)
    void OnButtonEvent(lv_obj_t* obj, lv_event_t event);

private:
    // Hier deklarierst du deine UI-Objekt-Pointer
    Pinetime::Controllers::FS& fs;
    pr::IFileFactoryPtr fileFactory;
    lv_obj_t* buttons;
    lv_obj_t* result;

    void delAndLog(const char* name);
};

}
template <>
struct AppTraits<Apps::ShoppingListAdmin> {
    static constexpr Apps app = Apps::ShoppingListAdmin;
    static constexpr const char* icon = Screens::Symbols::plug;
    static Screens::Screen* Create(AppControllers& controllers) {
        return new Screens::ShoppingListAdmin(controllers.filesystem);
    }
    static bool IsAvailable(Pinetime::Controllers::FS& filesystem) {
        return true;
    };
};
}
}
