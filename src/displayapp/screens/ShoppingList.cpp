#include "displayapp/screens/ShoppingList.h"
#include "components/fs/FS.h"

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

using namespace Pinetime::Applications::Screens;

// static lv_obj_t* preview_label(lv_obj_t* parent, const char* text) {
//     lv_obj_t* l = lv_label_create(parent, NULL);
//     // Lokale Textfarbe für das Label auf SCHWARZ setzen (Hauptteil, Standardzustand)
//     lv_obj_set_style_local_text_color(l, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);
//     lv_label_set_text(l, text);
//     return l;
// }


#if 1
static void eventHandler(lv_obj_t* obj, lv_event_t event) {
    auto app = static_cast<ShoppingList*>(obj->user_data);
    app->OnButtonEvent(obj, event);
}
#endif

constexpr const char* const tSkip = "SKIP";
constexpr const char* const tBw = "<";
constexpr const char* const tFw = ">";
constexpr const char* const tMvBw = "<-";
constexpr const char* const tMvFw = "->";
static const char* map[] = { tSkip, tBw, tFw, tMvBw, tMvFw, "" };

using Pinetime::Controllers::FS;

// bool exists(FS& fs, const char* path) {
//     lfs_info info;
//     int code = fs.Stat(path, &info);
//     log("code of stat %d", code);
//     log("type: %d", info.type);
//     return code >= 0;
// }

ShoppingList::ShoppingList(FS& fs) : fs(fs), fileFactory(new pr::RealFileFactory(fs)), doneState(*fileFactory), textWindow()
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
    pr::splitPacket(*fileFactory);
    fs.FileDelete(pr::fileExtIn);
    doneState.init();
    textWindow.init(*fileFactory);

    if (!(doneState.fileExists() && textWindow.isFileOpen())) {
        lv_obj_t* none = lv_label_create(lv_scr_act(), nullptr);
        lv_label_set_long_mode(none, LV_LABEL_LONG_BREAK);
        lv_obj_set_size(none, 240, 240);
        lv_obj_align(none, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);

        char s[32];
        pr::FixedStream ss(s, 32);
        ss << Pinetime::Version::VersionString() << " - Keine Einkaufsliste.\nBitte vom Handy\nper BT uebertragen.";
        lv_label_set_text(none, s);
        return;
    }
#endif

    // 0. Post-View
    // Neue Vorschau als nur ein Text mit allen Elementen durch "," getrennt:
    lv_obj_t* postview_cont = lv_cont_create(lv_scr_act(), NULL);
    const int heightPostview = 25;
    lv_obj_set_size(postview_cont, 240, heightPostview);
    lv_obj_align(postview_cont, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);
    lv_cont_set_fit2(postview_cont, LV_FIT_NONE, LV_FIT_NONE);
    // lv_cont_set_layout(preview_cont, LV_LAYOUT_COLUMN_LEFT); // Ordnet Labels untereinander an

    postview = lv_label_create(postview_cont, NULL);
    lv_label_set_long_mode(postview, LV_LABEL_LONG_BREAK);
    lv_obj_set_width(postview, 220);
    // Den automatischen Zeilenumbruch für Fließtext aktivieren
    lv_label_set_text(postview, "Bla, Aepfel, Birnen, Rapsölbutter, Zimt, Ketchup");
    // lv_label_set_text(preview, "Aepfel");
    lv_obj_set_style_local_text_color(postview, LV_LABEL_PART_MAIN, LV_STATE_DEFAULT, LV_COLOR_BLACK);


    // 1. DER RIESEN-BUTTON (Aktuelles Item)
    done_btn = lv_btn_create(lv_scr_act(), NULL);
    done_btn->user_data = this;
#if 1
    lv_obj_set_event_cb(done_btn, eventHandler);
#endif
    // lv_obj_set_size(done_btn, 240, 110); // Halbe Bildschirmhöhe
    const lv_coord_t additionalHeightFirst = -30;
    lv_obj_set_size(done_btn, 240, 110 + additionalHeightFirst); // Nicht so hoch
    lv_obj_align(done_btn, postview_cont, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);

    // Text im Riesen-Button zentrieren
    done_label = lv_label_create(done_btn, NULL);
    lv_label_set_text(done_label, "Bananen");
    // Textumbruch aktivieren, falls das Wort zu lang ist
    lv_label_set_long_mode(done_label, LV_LABEL_LONG_BREAK);
    lv_obj_set_width(done_label, 220);
    lv_obj_align(done_label, NULL, LV_ALIGN_CENTER, 0, 0);

    // 2. KONTROLL-BUTTONS (Mittlere Zeile)
    btnRow = lv_btnmatrix_create(lv_scr_act(), nullptr);
    btnRow->user_data = this;
#if 1
    lv_obj_set_event_cb(btnRow, eventHandler);
#endif
    lv_obj_align(btnRow, done_btn, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5);
    lv_btnmatrix_set_map(btnRow, map);
    lv_btnmatrix_set_btn_width(btnRow, 0, 4);
    lv_btnmatrix_set_btn_width(btnRow, 1, 2);
    lv_btnmatrix_set_btn_width(btnRow, 2, 2);
    lv_btnmatrix_set_btn_width(btnRow, 3, 2);
    lv_btnmatrix_set_btn_width(btnRow, 4, 2);
    lv_obj_set_size(btnRow, 240, 40);
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

ShoppingList::~ShoppingList() {
    // LVGL aufräumen, wenn der User die App über den physischen Button verlässt
    lv_obj_clean(lv_scr_act());
    log("Ende ~ShoppingList");
}

#if 1
void ShoppingList::OnButtonEvent(lv_obj_t* obj, lv_event_t event) {
    if (event == LV_EVENT_PRESSED) {
        if ((obj == this->done_btn)) {
            LFS_WARN("done clicked");
            if (textWindow.item(doneState.pos()) != "") {
                doneState.done();
                updateLabels();
            }
            else {
                LFS_WARN("done ignored");
            }
        }
        else if (obj == btnRow) {
            log("click on btnRow");
            const char* t = lv_btnmatrix_get_active_btn_text(btnRow);
            if (t == tSkip) {
                LFS_WARN("skip clicked");
                if (textWindow.item(doneState.pos()) != "") {
                    doneState.skip();
                    updateLabels();
                }
                else {
                    LFS_WARN("skip ignored");
                }

            }
            else if (t == tBw) {
                LFS_WARN("< clicked");
                if (doneState.pos() > 0) {
                    doneState.undo();
                    updateLabels();
                }
                else {
                    LFS_WARN("< ignored");
                }
            }
            else if (t == tFw) {
                log("> clicked");
                int pos = doneState.pos();
                std::string_view next = textWindow.item(pos);
                if (next == "") {
                    log("> ignored");
                    return;
                }
                if (doneState.isDone()) {
                    doneState.done();
                }
                else {
                    doneState.skip();
                }
                updateLabels();
            }
            else if (t == tMvFw) {
                LFS_WARN("-> clicked");
                int pos = doneState.pos();
                std::string_view next = textWindow.item(pos + 1);
                if (next == "") {
                    LFS_WARN("-> ignored");
                    return;
                }
                textWindow.swap(pos);
                doneState.forward();
                updateLabels();
            }
            else if (t == tMvBw) {
                LFS_WARN("<- clicked");
                int pos = doneState.pos();
                if (pos < 1 || textWindow.item(pos) == "") {
                    LFS_WARN("<- ignored");
                    return;
                }
                textWindow.swap(pos - 1);
                doneState.backward();
                updateLabels();
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
static lv_btnmatrix_ctrl_t btnState(bool disabled) {
    return disabled ? LV_BTNMATRIX_CTRL_DISABLED : 0;
}

void ShoppingList::updateLabels() {
    auto pos = doneState.pos();
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
            ss << (doneState.isDone(i) ? 'X' : '_');
            ss << ' ';
            ss << textWindow.item(i);
        }
        lv_label_set_text(postview, this->buf.data());
    }
    std::string item(textWindow.item(pos));
    bool skipDisabled = false, bwDisabled = false, fwDisabled = false, mvBwDisabled = false, mvFwDisabled = false;
    pr::FixedStream ss(buf.data(), buf.size());
    if (item == "") {
        ss << "Fertig!";
        lv_btn_set_state(done_btn, LV_BTN_STATE_DISABLED);
        fwDisabled = true;
        skipDisabled = true;
    }
    else {
        ss << "(";
        ss << pos + 1;
        ss << ") ";
        ss << '[';
        if (doneState.isDone()) ss << 'X';
        else if (doneState.isSkipped()) ss << ' ';
        else ss << ' ';
        ss << "] ";
        ss << item;
        lv_btn_set_state(done_btn, LV_BTN_STATE_RELEASED);
    }


    if (pos < 1) {
        bwDisabled = true;
        mvBwDisabled = true;
    }
    if (textWindow.item(pos) == "") {
        mvBwDisabled = true;
    }
    std::string_view next = textWindow.item(pos + 1);
    if (next == "") {
        mvFwDisabled = true;
    }

    lv_btnmatrix_ctrl_t btnRowCtrls[] = { btnState(skipDisabled), btnState(bwDisabled), btnState(fwDisabled), btnState(mvBwDisabled), btnState(mvFwDisabled) };
    lv_btnmatrix_set_ctrl_map(btnRow, btnRowCtrls);
    lv_btnmatrix_set_btn_width(btnRow, 0, 4);
    lv_btnmatrix_set_btn_width(btnRow, 1, 2);
    lv_btnmatrix_set_btn_width(btnRow, 2, 2);
    lv_btnmatrix_set_btn_width(btnRow, 3, 2);
    lv_btnmatrix_set_btn_width(btnRow, 4, 2);

    lv_label_set_text(done_label, buf.data());
    ss.clear();
    for (int i = pos + 1; ss.size() < 18 * 4 && item != ""; ++i) {
        if (i > pos + 1) {
            ss << ", ";
        }
        item = textWindow.item(i);
        if (item != "") {
            ss << (doneState.isDone(i) ? 'X' : '_');
            ss << ' ';
            ss << item;
        }
    }

    log("preview string '%s'", buf.data());
    lv_label_set_text(preview, buf.data());

}
#endif
