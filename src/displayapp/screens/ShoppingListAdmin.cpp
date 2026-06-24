#include "prUtils.h"
#include "displayapp/screens/ShoppingListAdmin.h"
#include "components/fs/FS.h"

#include "prLog.h"
#include "prRealFiles.h"
#include "prFixedStream.h"

#include "FreeRTOS.h"
#include "task.h"

#include <bitset>
#include <string>
#include <sstream>

#include <displayapp/screens/prLog.h>

using namespace Pinetime::Applications::Screens;
using namespace pr;

// static lv_obj_t* preview_label(lv_obj_t* parent, const char* text) {
//     lv_obj_t* l = lv_label_create(parent, NULL);
//     // Lokale Textfarbe für das Label auf SCHWARZ setzen (Hauptteil, Standardzustand)
//     lv_obj_set_style_local_text_color(l, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
//     lv_label_set_text(l, text);
//     return l;
// }


static void eventHandler(lv_obj_t* obj, lv_event_t event) {
    auto app = static_cast<ShoppingListAdmin*>(obj->user_data);
    app->OnButtonEvent(obj, event);
}

constexpr const char* const tDelAll = "DEL ALL";
constexpr const char* const tExtIn = ".ext.in";
constexpr const char* const tSplit = "SPLIT";
constexpr const char* const tExtOut = ".ext.out";
constexpr const char* const tLoop = "LOOP";

static const char* map[] = {
tDelAll, "\n",
tExtIn, tSplit, "\n",
tExtOut, tLoop,
""
};

ShoppingListAdmin::ShoppingListAdmin(Pinetime::Controllers::FS& fs)
    : fs(fs),
    fileFactory(new pr::RealFileFactory(fs)),
    buttons(nullptr)
{
    buttons = lv_btnmatrix_create(lv_scr_act(), nullptr);
    buttons->user_data = this;
    lv_obj_set_event_cb(buttons, eventHandler);
    lv_obj_align(buttons, nullptr, LV_ALIGN_IN_TOP_MID, 0, 0);
    lv_btnmatrix_set_map(buttons, map);
    // lv_btnmatrix_set_hei
    lv_obj_set_size(buttons, 220, 240);

}

ShoppingListAdmin::~ShoppingListAdmin() {
    // LVGL aufräumen, wenn der User die App über den physischen Button verlässt
    lv_obj_clean(lv_scr_act());
    log("Ende ~ShoppingListAdmin");
}

void ShoppingListAdmin::delAndLog(const char* name) {
    int code = fs.FileDelete(name);
    log("%s: %d", name, code);
}

void ShoppingListAdmin::OnButtonEvent(lv_obj_t* obj, lv_event_t event) {
    if (obj == buttons) {
        // log("Klick auf buttons");
        switch (event) {
        case LV_EVENT_PRESSED:
            log("pressed");
            break;
        case LV_EVENT_PRESS_LOST:
            log("press lost");
            break;
        case LV_EVENT_SHORT_CLICKED:
            log("short clicked");
            break;
        case LV_EVENT_LONG_PRESSED:
            log("long pressed");
            break;
        case LV_EVENT_CLICKED: {
            log("clicked");
            auto t = lv_btnmatrix_get_active_btn_text(buttons);
            if (t == tDelAll) {
                delAndLog(fileExtTmp);
                delAndLog(fileExtIn);
                delAndLog(fileDone);
                delAndLog(fileTxt);
                delAndLog(fileTmp);
                delAndLog(fileExtOut);
            }
            else if (t == tExtIn) {
                auto f = this->fileFactory->open(fileExtTmp, pr::IFileFactory::WRONLY | pr::IFileFactory::TRUNC | pr::IFileFactory::CREAT);
                if (!f) {
                    log("Konnte /shoppingList.ext.tmp nicht erzeugen");
                    return;
                }
                pr::writeExample(f);
                f.reset();
                log("/shoppingList.ext.tmp generiert");
                int code = fs.Rename(fileExtTmp, fileExtIn);
                log("Rename to /shoppingList.ext.in: %d", code);
            }
            else if (t == tSplit) {
                splitPacket(*fileFactory);
            }
            else if (t == tExtOut) {
                concatPacket(*fileFactory);
                log("Paket konkateniert");
                int code = fs.Rename(fileTmp, fileExtOut);
                log("Rename %s to %s: %d", fileTmp, fileExtOut, code);
            }
            else if (t == tLoop) {
                int code = fs.Rename(fileExtOut, fileExtIn);
                log("Rename of loop: %d", code);
            }
            break;
        }
        case LV_EVENT_VALUE_CHANGED:
            log("value changed");
            break;

            // default:
                // log("unknown: %d", event);
                // break;
        }
    }
}

// class MyStream {
// private:
//     std::string s;

// public:
//     MyStream();
//     ~MyStream();
//     void clear();
//     MyStream& operator<<(char c);
//     MyStream& operator<<(std::string_view&& sv);
//     /**
//      * nur für i >= 0
//      */
//     MyStream& operator<<(int i);
//     const char* c_str() const;
//     std::string::size_type size() const;
// };

// MyStream::MyStream() : s() {}
// MyStream::~MyStream() {}
// void MyStream::clear() {
//     s.clear();
// }
// MyStream& MyStream::operator<<(char c) {
//     s += c;
//     return *this;
// }
// MyStream& MyStream::operator<<(std::string_view&& sv) {
//     std::string sv1(sv);
//     s += sv1;
//     return *this;
// }
// MyStream& MyStream::operator<<(int i) {
//     if (i == -2147483648) {
//         s += "-2147483648";
//         return *this;
//     }
//     if (i < 0) {
//         s += '-';
//         return *this << (-i);
//     }
//     assert(i >= 0);
//     auto pos = s.size();
//     while (i > 9) {
//         std::string::value_type c = static_cast<std::string::value_type>((int)'0' + (i % 10));
//         s.insert(s.begin() + pos, c);
//         i /= 10;
//     }
//     std::string::value_type c = static_cast<std::string::value_type>((int)'0' + (i % 10));
//     s.insert(s.begin() + pos, c);
//     return *this;
// }
// const char* MyStream::c_str() const {
//     return s.c_str();
// }
// std::string::size_type MyStream::size() const {
//     return s.size();
// }
