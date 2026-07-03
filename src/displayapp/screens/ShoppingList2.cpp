#include "displayapp/screens/ShoppingList2.h"
#include "components/fs/FS.h"
#include "displayapp/screens/prAmountState.h"
#include "prUtils.h"
#include "prLog.h"
#include "prRealFiles.h"
#include "prFixedStream.h"

#include "FreeRTOS.h"
#include "task.h"
#include "Version.h"

#include <bitset>
#include <string>
#include <sstream>

#include <displayapp/screens/prLog.h>

// extern "C" {
//     // extern lv_font_t jetbrains_mono_24;
//     extern lv_font_t jetbrains_mono_bold_20;
// }

using namespace Pinetime::Applications::Screens;

void labelBgColor(lv_obj_t* label, uint8_t red, uint8_t green, uint8_t blue) {
    // 1. Hintergrund deckend machen (0 = transparent, 255 = komplett deckend)
    lv_obj_set_style_local_bg_opa(label, LV_LABEL_PART_MAIN, 0, LV_OPA_COVER);

    // 2. Hintergrundfarbe setzen (z. B. Rot)
    auto color = LV_COLOR_MAKE(red, green, blue);
    lv_obj_set_style_local_bg_color(label, LV_LABEL_PART_MAIN, 0, color);
}

void labelTextColor(lv_obj_t* label, uint8_t red, uint8_t green, uint8_t blue) {
    auto color = LV_COLOR_MAKE(red, green, blue);
    lv_obj_set_style_local_text_color(label, LV_BTN_PART_MAIN, 0, color);
}

void buttonmatrixBgColor(lv_obj_t* matrix, uint8_t red, uint8_t green, uint8_t blue) {

    lv_obj_set_style_local_bg_opa(matrix, LV_BTNMATRIX_PART_BTN, 0, LV_OPA_COVER);
    auto color = LV_COLOR_MAKE(red, green, blue);
    lv_obj_set_style_local_bg_color(matrix, LV_BTNMATRIX_PART_BTN, 0, color);

}

// static lv_obj_t* preview_label(lv_obj_t* parent, const char* text) {
//     lv_obj_t* l = lv_label_create(parent, NULL);
//     // Lokale Textfarbe für das Label auf SCHWARZ setzen (Hauptteil, Standardzustand)
//     lv_obj_set_style_local_text_color(l, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
//     lv_label_set_text(l, text);
//     return l;
// }


#if 1
static void eventHandler(lv_obj_t* obj, lv_event_t event) {
    auto app = static_cast<ShoppingList2*>(obj->user_data);
    app->OnButtonEvent(obj, event);
}
#endif

constexpr const char* const tMod = "MOD";
constexpr const char* const tBw = "<";
constexpr const char* const tFw = ">";
constexpr const char* const tMvBw = "<-";
constexpr const char* const tMvFw = "->";
// static const char* map[] = { tBw, tFw, tMvBw, tMvFw, "" };
static const char* mapMod0[] = { tBw, tMod, tFw, "" };
static const char* mapMod1[] = { tMvBw, tMod, tMvFw, "" };

constexpr int numModes = 2;
static const char** modeMaps[] = {
    mapMod0,
    mapMod1
};

using Pinetime::Controllers::FS;

// bool exists(FS& fs, const char* path) {
//     lfs_info info;
//     int code = fs.Stat(path, &info);
//     log("code of stat %d", code);
//     log("type: %d", info.type);
//     return code >= 0;
// }

ShoppingList2::ShoppingList2(FS& fs) : fs(fs), fileFactory(new pr::RealFileFactory(fs)), amountState(*fileFactory), textWindow(), mode(0)
{
    // // Neue Vorschau als nur ein Text mit allen Elementen durch "," getrennt:
    // lv_obj_t* postview_cont = lv_cont_create(lv_scr_act(), NULL);
    // const int heightPostview = 25;
    // lv_obj_set_size(postview_cont, 240, heightPostview);
    // lv_obj_align(postview_cont, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);
    // lv_cont_set_fit2(postview_cont, LV_FIT_NONE, LV_FIT_NONE);

    // postview = lv_label_create(postview_cont, NULL);
    // lv_label_set_long_mode(postview, LV_LABEL_LONG_BREAK);
    // lv_obj_set_width(postview, 220);
    // // Den automatischen Zeilenumbruch für Fließtext aktivieren
    // lv_label_set_text(postview, "v1.16.7 Rapsölbutter, Zimt, Ketchup");
    // // lv_label_set_text(preview, "Aepfel");
    // lv_obj_set_style_local_text_color(postview, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);



    // Wenn vorhanden, `pr::fileExtIn` auspacken:
#if 1
    pr::splitPacket3(*fileFactory);
    fs.FileDelete(pr::fileExtIn);
    amountState.init();
    textWindow.init(*fileFactory);

    if (!(amountState.fileExists() && textWindow.isFileOpen())) {
        lv_obj_t* none = lv_label_create(lv_scr_act(), nullptr);
        lv_label_set_long_mode(none, LV_LABEL_LONG_BREAK);
        lv_obj_set_size(none, 240, 240);

        std::array<char, 128> s;
        pr::FixedStream ss(s.data(), s.size());
        ss << Pinetime::Version::VersionString() << "\nKeine Einkaufsliste.\nBitte vom Handy per\nBT uebertragen.";
        lv_label_set_text(none, s.data());
        lv_obj_align(none, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);
        return;
    }
#endif

    // 0. Post-View
    // Neue Vorschau als nur ein Text mit allen Elementen durch "," getrennt:
#if 1
    postview = lv_label_create(lv_scr_act(), NULL);
    labelTextColor(postview, 0xff, 0xf5, 0x70);
    // {
    //     // 1. Hintergrund deckend machen (0 = transparent, 255 = komplett deckend)
    //     lv_obj_set_style_local_bg_opa(postview, LV_LABEL_PART_MAIN, 0, LV_OPA_COVER);

    //     // 2. Hintergrundfarbe setzen (z. B. Rot)
    //     auto color = LV_COLOR_MAKE(0xdd, 0xd0, 0x17);
    //     lv_obj_set_style_local_bg_color(postview, LV_LABEL_PART_MAIN, 0, color);

    // }
    lv_label_set_long_mode(postview, LV_LABEL_LONG_BREAK);
    // lv_obj_set_width(postview, 240);
    const int heightPostview = 25;
    lv_obj_set_size(postview, 240, heightPostview);
#else
    lv_obj_t* postview_cont = lv_cont_create(lv_scr_act(), NULL);
    const int heightPostview = 25;
    lv_obj_set_size(postview_cont, 240, heightPostview);
    lv_obj_align(postview_cont, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);
    lv_cont_set_fit2(postview_cont, LV_FIT_NONE, LV_FIT_NONE);
    // lv_cont_set_layout(preview_cont, LV_LAYOUT_COLUMN_LEFT); // Ordnet Labels untereinander an

    postview = lv_label_create(postview_cont, NULL);
    lv_label_set_long_mode(postview, LV_LABEL_LONG_BREAK);
    lv_obj_set_width(postview, 240);
    // Den automatischen Zeilenumbruch für Fließtext aktivieren
    lv_label_set_text(postview, "Bla, Aepfel, Birnen, Rapsölbutter, Zimt, Ketchup");
    // lv_label_set_text(preview, "Aepfel");
    lv_obj_set_style_local_text_color(postview, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
#endif

    // 1. DER RIESEN-BUTTON (Aktuelles Item)
    done_btn = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_set_style_local_radius(done_btn, LV_BTN_PART_MAIN, LV_BTN_STATE_RELEASED, 0);
    // {
    //     // 1. Hintergrund deckend machen (0 = transparent, 255 = komplett deckend)
    //     lv_obj_set_style_local_bg_opa(done_btn, LV_BTN_PART_MAIN, 0, LV_OPA_COVER);

    //     // 2. Hintergrundfarbe setzen (z. B. Rot)
    //     auto color = LV_COLOR_MAKE(0x58, 0xba, 0x3e);
    //     lv_obj_set_style_local_bg_color(done_btn, LV_LABEL_PART_MAIN, 0, color);

    // }
    done_btn->user_data = this;
#if 1
    lv_obj_set_event_cb(done_btn, eventHandler);
#endif
    // lv_obj_set_size(done_btn, 240, 110); // Halbe Bildschirmhöhe
    const lv_coord_t additionalHeightFirst = -30;
    int heightDone = 110 + additionalHeightFirst;
    lv_obj_set_size(done_btn, 240, heightDone - 1); // Nicht so hoch
#if 1
#else
    lv_obj_align(done_btn, postview_cont, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);
#endif

    // Text im Riesen-Button zentrieren
    done_label = lv_label_create(done_btn, NULL);
    labelBgColor(done_btn, 20, 20, 20);
    labelTextColor(done_label, 0x80, 0xff, 0x5d);
    lv_label_set_text(done_label, "Bananen");
    // Textumbruch aktivieren, falls das Wort zu lang ist
    lv_label_set_long_mode(done_label, LV_LABEL_LONG_BREAK);
    lv_obj_set_width(done_label, 220);
    lv_obj_align(done_label, NULL, LV_ALIGN_CENTER, 0, 0);
    lv_obj_align(done_btn, postview, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);

    // // Skip-Button (Links)
    // this->skip_btn = lv_btn_create(btnRow, NULL);
    // skip_btn->user_data = this;
    // lv_obj_set_event_cb(skip_btn, eventHandler);
    // // lv_obj_set_size(skip_btn, 115, 40);
    // // lv_obj_align(skip_btn, done_btn, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5);

    // lv_obj_t* skip_label = lv_label_create(skip_btn, NULL);
    // lv_label_set_text(skip_label, "SKIP");

    // // Undo-Button (Rechts)
    // this->undo_btn = lv_btn_create(btnRow, NULL);
    // undo_btn->user_data = this;
    // lv_obj_set_event_cb(undo_btn, eventHandler);
    // lv_obj_t* undo_label = lv_label_create(undo_btn, NULL);
    // lv_label_set_text(undo_label, "UNDO");

    // // BEGIN KI
    // // 1. Label direkt auf dem Screen erstellen (kein Container, der stören kann)
    // lv_obj_t* preview = lv_label_create(lv_scr_act(), NULL);

    // // 2. Modus festlegen
    // lv_label_set_long_mode(preview, LV_LABEL_LONG_SROLL);

    // // 3. Breite knallhart auf dem Label festlegen
    // lv_obj_set_width(preview, 220);

    // // 4. Position manuell festmeißeln (X=10 für Zentrierung bei 240 Gesamtbreite)
    // lv_obj_set_x(preview, 10);
    // lv_obj_set_y(preview, 160);

    // // 5. Textfarbe setzen
    // lv_obj_set_style_local_text_color(preview, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_WHITE);

    // // 6. ERST JETZT den Text übergeben
    // lv_label_set_text(preview, "Bla, Aepfel, Birnen, Rapsölbutter, Zimt, Ketchup");
    // // ENDE KI

    // Neue Vorschau als nur ein Text mit allen Elementen durch "," getrennt:
#if 1
    preview = lv_label_create(lv_scr_act(), NULL);
    int heightPreview = 80 - additionalHeightFirst - heightPostview;
    lv_label_set_long_mode(preview, LV_LABEL_LONG_BREAK);
    log("heightPreview %d", heightPreview);
    lv_obj_set_size(preview, 240, heightPreview);
    lv_obj_align(preview, done_btn, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);
#else
    lv_obj_t* preview_cont = lv_cont_create(lv_scr_act(), NULL);
    lv_obj_set_size(preview_cont, 240, 75 - additionalHeightFirst - heightPostview);
    lv_obj_align(preview_cont, NULL, LV_ALIGN_IN_BOTTOM_MID, 0, 0);
    lv_cont_set_fit2(preview_cont, LV_FIT_NONE, LV_FIT_NONE);
    // lv_cont_set_layout(preview_cont, LV_LAYOUT_COLUMN_LEFT); // Ordnet Labels untereinander an

    preview = lv_label_create(preview_cont, NULL);
    lv_label_set_long_mode(preview, LV_LABEL_LONG_BREAK);
    lv_obj_set_width(preview, 220);
    // Den automatischen Zeilenumbruch für Fließtext aktivieren
    lv_label_set_text(preview, "Bla, Aepfel, Birnen, Rapsölbutter, Zimt, Ketchup");
    // lv_label_set_text(preview, "Aepfel");
    lv_obj_set_style_local_text_color(preview, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
#endif

    // 2. KONTROLL-BUTTONS (Mittlere Zeile)
    btnRow = lv_btnmatrix_create(lv_scr_act(), nullptr);
    btnRow->user_data = this;
#if 1
    lv_obj_set_event_cb(btnRow, eventHandler);
#endif
    lv_btnmatrix_set_map(btnRow, modeMaps[mode]);
    // width and others will be set in updateLabels():
    // lv_btnmatrix_set_btn_width(btnRow, 0, 3);
    // lv_btnmatrix_set_btn_width(btnRow, 1, 3);
    // lv_btnmatrix_set_btn_width(btnRow, 2, 3);
    // lv_btnmatrix_set_btn_width(btnRow, 3, 3);
    {
        // buttonmatrixColor(btnRow, 255, 0, 0);
        buttonmatrixBgColor(btnRow, 20, 20, 20);
    }
    int heightBtnRow = 76;
    lv_obj_set_size(btnRow, 240, heightBtnRow);
    {
        static lv_style_t matrix_style;
        lv_style_init(&matrix_style);

        // Setzt den inneren Button-Abstand auf 5 Pixel
        lv_style_set_pad_left(&matrix_style, LV_STATE_DEFAULT, 0);
        lv_style_set_pad_right(&matrix_style, LV_STATE_DEFAULT, 0);
        lv_style_set_pad_top(&matrix_style, LV_STATE_DEFAULT, 0);
        lv_style_set_pad_bottom(&matrix_style, LV_STATE_DEFAULT, 0);
        lv_style_set_pad_hor(&matrix_style, LV_STATE_DEFAULT, 1);
        lv_obj_add_style(btnRow, LV_BTNMATRIX_PART_BG, &matrix_style);
    }
    {
        static lv_style_t matrix_style;
        lv_style_init(&matrix_style);

        lv_style_set_radius(&matrix_style, LV_STATE_DEFAULT, 0);
        lv_obj_add_style(btnRow, LV_BTNMATRIX_PART_BTN, &matrix_style);

    }
    lv_obj_align(btnRow, lv_scr_act(), LV_ALIGN_IN_BOTTOM_LEFT, 0, 0);

    // log("heightPostview %d, heightDone %d, heightBtnRow %d, heightPreview %d, sum of heights %d", heightPostview, heightDone, heightBtnRow, heightPreview, heightPostview + heightDone + heightBtnRow + heightPreview);

#if 1
    updateLabels();
#endif

#if 0
    {
        lv_obj_t* versionLabel = lv_label_create(lv_scr_act(), NULL);
        lv_obj_align(versionLabel, NULL, LV_ALIGN_IN_BOTTOM_LEFT, 0, 0);

        char s[32];
        pr::FixedStream ss(s, 32);
        ss << Pinetime::Version::VersionString() << " - Keine Einkaufsliste.\nBitte vom Handy\nper BT uebertragen.";

        lv_label_set_text(versionLabel, s);
    }
#endif
}

ShoppingList2::~ShoppingList2() {
    // LVGL aufräumen, wenn der User die App über den physischen Button verlässt
    lv_obj_clean(lv_scr_act());
    log("Ende ~ShoppingList2");
}

#if 1
void ShoppingList2::OnButtonEvent(lv_obj_t* obj, lv_event_t event) {
    if (event == LV_EVENT_SHORT_CLICKED) {
        if ((obj == this->done_btn)) {
            LFS_WARN("done clicked");
            uint32_t amount = amountState.getAmount();
            if (!textWindow.item(amountState.pos()).empty()) {
                if (amount > 0) {
                    amountState.setAmount(amount - 1);
                }
                else {
                    amountState.nextPos();
                }
                updateLabels();
            }
            else {
                LFS_WARN("done ignored");
            }
        }
        else if (obj == btnRow) {
            log("click on btnRow");
            const char* t = lv_btnmatrix_get_active_btn_text(btnRow);
            if (t == tBw) {
                LFS_WARN("< clicked");
                if (amountState.pos() > 0) {
                    amountState.prevPos();
                    updateLabels();
                }
                else {
                    LFS_WARN("< ignored");
                }
            }
            else if (t == tFw) {
                log("> clicked");
                int pos = amountState.pos();
                std::string_view next = textWindow.item(pos);
                if (next.empty()) {
                    log("> ignored");
                    return;
                }
                amountState.nextPos();
                updateLabels();
            }
            else if (t == tMvFw) {
                LFS_WARN("-> clicked");
                int pos = amountState.pos();
                std::string_view next = textWindow.item(pos + 1);
                if (next.empty()) {
                    LFS_WARN("-> ignored");
                    return;
                }
                textWindow.swap(pos, buf);
                amountState.forward();
                updateLabels();
            }
            else if (t == tMvBw) {
                LFS_WARN("<- clicked");
                int pos = amountState.pos();
                if (pos < 1 || textWindow.item(pos).empty()) {
                    LFS_WARN("<- ignored");
                    return;
                }
                textWindow.swap(pos - 1, buf);
                amountState.backward();
                updateLabels();
            }
            else if (t == tMod) {
                this->mode = (this->mode + 1) % numModes;
                lv_btnmatrix_set_map(btnRow, modeMaps[mode]);
                updateLabels();
            }
        }
    }
    else if (event == LV_EVENT_LONG_PRESSED) {
        if ((obj == this->done_btn)) {
            LFS_WARN("done long pressed");
            uint32_t amount = amountState.getAmount();
            if (!textWindow.item(amountState.pos()).empty()) {
                amountState.setAmount(amount + 1);
                updateLabels();
            }
            else {
                LFS_WARN("done long press ignored");
            }
        }
    }

}
#endif

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

#if 1
static lv_btnmatrix_ctrl_t btnState(bool disabled, int width = 0) {
    return static_cast<lv_btnmatrix_ctrl_t>((disabled ? LV_BTNMATRIX_CTRL_DISABLED : 0) | width);
}

static pr::FixedStream& formatItem(pr::FixedStream& ss, std::string_view item) {
    // log("item '%.*s'", (int)item.length(), item.data());
    auto begin = item.begin();
    auto end = begin;
    while (end != item.end() && *end != '\t') ++end;
    // log("name: end - begin = %d", (int)(end - begin));
    std::string_view name(begin, end);
    begin = end;
    if (begin != item.end()) ++begin;
    end = begin;
    while (end != item.end() && *end != '\t') ++end;
    std::string_view unit(begin, end);
    begin = end;
    if (begin != item.end()) ++begin;
    end = begin;
    while (end != item.end() && *end != '\t') ++end;
    std::string_view comment(begin, end);
    // log("name '%.*s'", (int)name.length(), name.data());
    // log("unit '%.*s'", (int)unit.length(), unit.data());
    // log("comment '%.*s'", (int)comment.length(), comment.data());
    ss << unit;
    // log("name.length=%d", (int)name.length());
    ss << ' ';
    // log("name '%.*s'", (int)name.length(), name.data());
    ss.appendConverted(name);
    if (comment.length() > 0) {
        ss << ' ' << comment;
    }

    return ss;
}

void ShoppingList2::updateLabels() {
    auto pos = amountState.pos();
    LFS_WARN("pos %d", pos);
    {
        const int n = 1;
        int postviewBegin = std::max(0, pos - n);
        int postViewEnd = pos;
        pr::FixedStream ss(this->buf.data(), this->buf.size());
        for (int i = postviewBegin; i < postViewEnd; ++i) {
            if (i != postviewBegin) {
                ss << ", ";
            }
            ss << (int)amountState.getAmount(i) << "x";
            formatItem(ss, textWindow.item(i));
        }
        lv_label_set_text(postview, this->buf.data());
    }
    std::string_view item(textWindow.item(pos));
    bool bwDisabled = false, fwDisabled = false, mvBwDisabled = false, mvFwDisabled = false;
    pr::FixedStream ss(buf.data(), buf.size());
    if (item.empty()) {
        ss << "Fertig!";
        lv_btn_set_state(done_btn, LV_BTN_STATE_DISABLED);
        fwDisabled = true;
    }
    else {
        ss << "(";
        ss << pos + 1;
        ss << ") ";
        ss << (int)amountState.getAmount() << "x";
        formatItem(ss, item);

        // // std::string_view unit
        // ss << item;
        lv_btn_set_state(done_btn, LV_BTN_STATE_RELEASED);
    }

    lv_label_set_text(done_label, buf.data());

    if (pos < 1) {
        bwDisabled = true;
        mvBwDisabled = true;
    }
    if (textWindow.item(pos).empty()) {
        mvBwDisabled = true;
    }
    std::string_view next = textWindow.item(pos + 1);
    if (next.empty()) {
        mvFwDisabled = true;
    }

    switch (mode) {
    case 0: {
        lv_btnmatrix_ctrl_t btnRowCtrls[] = { btnState(bwDisabled, 1), btnState(false, 1), btnState(fwDisabled, 1) };
        lv_btnmatrix_set_ctrl_map(btnRow, btnRowCtrls);
        break;
    }
    case 1: {
        lv_btnmatrix_ctrl_t btnRowCtrls[] = { btnState(mvBwDisabled, 1), btnState(false, 1), btnState(mvFwDisabled, 1) };
        lv_btnmatrix_set_ctrl_map(btnRow, btnRowCtrls);
        break;
    }
    } // switch

// lv_btnmatrix_set_btn_width(btnRow, 0, 3);
// lv_btnmatrix_set_btn_width(btnRow, 1, 3);
// lv_btnmatrix_set_btn_width(btnRow, 2, 3);
// lv_btnmatrix_set_btn_width(btnRow, 3, 3);
    lv_btnmatrix_set_align(btnRow, LV_LABEL_ALIGN_LEFT);

    ss.clear();
    for (int i = pos + 1; ss.size() < 18 * 4 && item != ""; ++i) {
        if (i > pos + 1) {
            ss << ", ";
        }
        item = textWindow.item(i);
        if (item != "") {
            ss << (int)amountState.getAmount(i) << "x";
            formatItem(ss, item);
        }
    }

    log("preview string '%s'", buf.data());
    lv_label_set_text(preview, buf.data());

}
#endif