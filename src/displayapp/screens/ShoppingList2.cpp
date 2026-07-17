#include "displayapp/screens/ShoppingList2.hpp"
#include "components/fs/FS.h"
#include "displayapp/screens/prAmountState.hpp"
#include "prUtils.hpp"
#include "prLog.hpp"
#include "prRealFiles.hpp"
#include "prFixedStream.hpp"
#include "prLog.hpp"

#include "FreeRTOS.h"
#include "task.h"
#include "Version.h"

#include <bitset>
#include <string>
#include <sstream>

// extern "C" {
//     // extern lv_font_t jetbrains_mono_24;
//     extern lv_font_t jetbrains_mono_bold_20;
// }

using namespace Pinetime::Applications::Screens;

void labelBgColor(lv_obj_t* label, uint8_t red, uint8_t green, uint8_t blue)
{
    // 1. Hintergrund deckend machen (0 = transparent, 255 = komplett deckend)
    lv_obj_set_style_local_bg_opa(label, LV_LABEL_PART_MAIN, 0, LV_OPA_COVER);

    // 2. Hintergrundfarbe setzen (z. B. Rot)
    auto color = LV_COLOR_MAKE(red, green, blue);
    lv_obj_set_style_local_bg_color(label, LV_LABEL_PART_MAIN, 0, color);
}

void labelTextColor(lv_obj_t* label, uint8_t red, uint8_t green, uint8_t blue)
{
    auto color = LV_COLOR_MAKE(red, green, blue);
    lv_obj_set_style_local_text_color(label, LV_BTN_PART_MAIN, 0, color);
}

void buttonmatrixBgColor(lv_obj_t* matrix,
                         uint8_t red,
                         uint8_t green,
                         uint8_t blue)
{

    lv_obj_set_style_local_bg_opa(matrix,
                                  LV_BTNMATRIX_PART_BTN,
                                  0,
                                  LV_OPA_COVER);
    auto color = LV_COLOR_MAKE(red, green, blue);
    lv_obj_set_style_local_bg_color(matrix, LV_BTNMATRIX_PART_BTN, 0, color);
}

// static lv_obj_t* preview_label(lv_obj_t* parent, const char* text) {
//     lv_obj_t* l = lv_label_create(parent, NULL);
//     // Lokale Textfarbe für das Label auf SCHWARZ setzen (Hauptteil,
//     Standardzustand) lv_obj_set_style_local_text_color(l, LV_LABEL_PART_MAIN,
//     LV_STATE_DEFAULT, LV_COLOR_BLACK); lv_label_set_text(l, text); return l;
// }

static void eventHandler(lv_obj_t* obj, lv_event_t event)
{
    auto app = static_cast<ShoppingList2*>(lv_obj_get_user_data(obj));
    app->OnButtonEvent(obj, event);
}

// // Button Labels

// constexpr const char* const tMod = "MOD";
// constexpr const char* const tBw = "<";
// constexpr const char* const tFw = ">";
// constexpr const char* const tMvBw = "<-";
// constexpr const char* const tMvFw = "->";
// constexpr const char* const tFilt = "FILT";
// constexpr const char* const tSkip = "SKIP";

// static const char* map[] = { tBw, tFw, tMvBw, tMvFw, "" };

// Mode Enum

enum class Mode {
    GO,
    MOVE,
    SKIP,
};

// Footer Modes

// static const char* mapModGo[] = {tBw, tMod, tFw, ""};
// static const char* mapModMove[] = {tMvBw, tMod, tMvFw, ""};
// static const char* mapModSkip[] = {tFilt, tMod, tSkip, ""};

constexpr int numModes = 3;
// static const char** modeMaps[] = {
//     mapModGo,
//     mapModMove,
//     mapModSkip,
// };

using Pinetime::Controllers::FS;

constexpr int yPostview = 0;
constexpr int heightPostview = 55;
constexpr int yDone = yPostview + heightPostview;
constexpr int heightDone = 60;
constexpr int yPreview = yDone + heightDone;
constexpr int heightPreview = 55;
constexpr int yBtnRow = yPreview + heightPreview;
constexpr int heightBtnRow = 70;

// bool exists(FS& fs, const char* path) {
//     lfs_info info;
//     int code = fs.Stat(path, &info);
//     log("code of stat %d", code);
//     log("type: %d", info.type);
//     return code >= 0;
// }

ShoppingList2::ShoppingList2(FS& fs, Pinetime::Components::LittleVgl& lvgl)
    : fs(fs),
      fileFactory(new pr::RealFileFactory(fs)),
      postview(),
      done_btn(),
      btnRow(),
      matBtnRow(),
      db(*fileFactory, lvgl),
      //   mode(0),
      filter(false),
      taskRefresh(nullptr),
      lvglEventSource(),
      modList(lvglEventSource),
      modMenu(lvglEventSource)
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
    // lv_obj_set_style_local_text_color(postview, LV_LABEL_PART_MAIN,
    // LV_STATE_DEFAULT, LV_COLOR_BLACK);

    // Wenn vorhanden, `pr::fileExtIn` auspacken:
#if 1
    pr::splitPacket3(*fileFactory);
    fs.FileDelete(pr::fileExtIn);
    db.init(*fileFactory);

    if (!(db.amountState.fileExists() && db.textWindow.isFileOpen())) {
        lv_obj_t* none = lv_label_create(lv_scr_act(), nullptr);
        lv_label_set_long_mode(none, LV_LABEL_LONG_DOT);
        lv_obj_set_size(none, 240, 240);

        std::array<char, 128> s;
        pr::FixedStream ss(s.data(), s.size());
        ss << Pinetime::Version::VersionString()
           << "\nKeine Einkaufsliste.\nBitte vom Handy per\nBT uebertragen.";
        lv_label_set_text(none, s.data());
        lv_obj_align(none, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);
        return;
    }
#endif

    // 0. Post-View
    // Neue Vorschau als nur ein Text mit allen Elementen durch "," getrennt:
#if 1
    postview = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_long_mode(postview, LV_LABEL_LONG_DOT);
    labelTextColor(postview, 0xff, 0xf5, 0x70);
    // {
    //     // 1. Hintergrund deckend machen (0 = transparent, 255 = komplett
    //     deckend) lv_obj_set_style_local_bg_opa(postview, LV_LABEL_PART_MAIN,
    //     0, LV_OPA_COVER);

    //     // 2. Hintergrundfarbe setzen (z. B. Rot)
    //     auto color = LV_COLOR_MAKE(0xdd, 0xd0, 0x17);
    //     lv_obj_set_style_local_bg_color(postview, LV_LABEL_PART_MAIN, 0,
    //     color);

    // }
    // lv_obj_set_width(postview, 240);
    lv_obj_set_size(postview, 240, heightPostview);
    lv_obj_set_pos(postview, 0, yPostview);
    // lv_obj_align(postview, nullptr, LV_ALIGN_IN_TOP_LEFT, 0, 0);
#else
    lv_obj_t* postview_cont = lv_cont_create(lv_scr_act(), NULL);
    const int heightPostview = 25;
    lv_obj_set_size(postview_cont, 240, heightPostview);
    lv_obj_align(postview_cont, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);
    lv_cont_set_fit2(postview_cont, LV_FIT_NONE, LV_FIT_NONE);
    // lv_cont_set_layout(preview_cont, LV_LAYOUT_COLUMN_LEFT); // Ordnet Labels
    // untereinander an

    postview = lv_label_create(postview_cont, NULL);
    lv_label_set_long_mode(postview, LV_LABEL_LONG_BREAK);
    lv_obj_set_width(postview, 240);
    // Den automatischen Zeilenumbruch für Fließtext aktivieren
    lv_label_set_text(postview,
                      "Bla, Aepfel, Birnen, Rapsölbutter, Zimt, Ketchup");
    // lv_label_set_text(preview, "Aepfel");
    lv_obj_set_style_local_text_color(postview,
                                      LV_LABEL_PART_MAIN,
                                      LV_STATE_DEFAULT,
                                      LV_COLOR_BLACK);
#endif

    // 1. DER RIESEN-BUTTON (Aktuelles Item)
    done_btn = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_set_style_local_bg_color(done_btn,
                                    LV_BTN_PART_MAIN,
                                    LV_STATE_PRESSED,
                                    LV_COLOR_MAKE(80, 80, 80));
    lv_obj_set_style_local_radius(done_btn,
                                  LV_BTN_PART_MAIN,
                                  LV_BTN_STATE_RELEASED,
                                  0);
    // {
    //     // 1. Hintergrund deckend machen (0 = transparent, 255 = komplett
    //     deckend) lv_obj_set_style_local_bg_opa(done_btn, LV_BTN_PART_MAIN, 0,
    //     LV_OPA_COVER);

    //     // 2. Hintergrundfarbe setzen (z. B. Rot)
    //     auto color = LV_COLOR_MAKE(0x58, 0xba, 0x3e);
    //     lv_obj_set_style_local_bg_color(done_btn, LV_LABEL_PART_MAIN, 0,
    //     color);

    // }
    done_btn->user_data = this;
    lv_obj_set_event_cb(done_btn, eventHandler);
    lv_obj_set_size(done_btn, 240, heightDone); // Nicht so hoch
    // lv_obj_align(done_btn, postview, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);
    lv_obj_set_pos(done_btn, 0, yDone);

    // Text im Riesen-Button zentrieren
    done_label = lv_label_create(done_btn, NULL);
    lv_label_set_long_mode(done_label, LV_LABEL_LONG_DOT);
    lv_obj_set_size(done_label, 240, heightDone);
    lv_obj_set_pos(done_label, 0, 0);
    labelBgColor(done_btn, 20, 20, 20);
    labelTextColor(done_label, 0x80, 0xff, 0x5d);
    // lv_label_set_text(done_label, "Bananen");
    // Textumbruch aktivieren, falls das Wort zu lang ist
    lv_label_set_align(done_label, LV_LABEL_ALIGN_LEFT);
    // lv_obj_set_width(done_label, 220);
    // lv_obj_align(done_label, NULL, LV_ALIGN_CENTER, 0, 0);
    // lv_obj_set_pos(done_label, 0, heightPostview);

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
    // // 1. Label direkt auf dem Screen erstellen (kein Container, der stören
    // kann) lv_obj_t* preview = lv_label_create(lv_scr_act(), NULL);

    // // 2. Modus festlegen
    // lv_label_set_long_mode(preview, LV_LABEL_LONG_SROLL);

    // // 3. Breite knallhart auf dem Label festlegen
    // lv_obj_set_width(preview, 220);

    // // 4. Position manuell festmeißeln (X=10 für Zentrierung bei 240
    // Gesamtbreite) lv_obj_set_x(preview, 10); lv_obj_set_y(preview, 160);

    // // 5. Textfarbe setzen
    // lv_obj_set_style_local_text_color(preview, LV_LABEL_PART_MAIN,
    // LV_STATE_DEFAULT, LV_COLOR_WHITE);

    // // 6. ERST JETZT den Text übergeben
    // lv_label_set_text(preview, "Bla, Aepfel, Birnen, Rapsölbutter, Zimt,
    // Ketchup");
    // // ENDE KI

    // Neue Vorschau als nur ein Text mit allen Elementen durch "," getrennt:
#if 1
    preview = lv_label_create(lv_scr_act(), NULL);
    lv_label_set_long_mode(preview, LV_LABEL_LONG_DOT);
    log("heightPreview %d", heightPreview);
    lv_obj_set_size(preview, 240, heightPreview);
    // lv_obj_align(preview, done_btn, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 0);
    lv_obj_set_pos(preview, 0, yPreview);
#else
    // lv_obj_t* preview_cont = lv_cont_create(lv_scr_act(), NULL);
    // lv_obj_set_size(preview_cont, 240, 75 - additionalHeightFirst -
    // heightPostview); lv_obj_align(preview_cont, NULL, LV_ALIGN_IN_BOTTOM_MID,
    // 0, 0); lv_cont_set_fit2(preview_cont, LV_FIT_NONE, LV_FIT_NONE);
    // // lv_cont_set_layout(preview_cont, LV_LAYOUT_COLUMN_LEFT); // Ordnet
    // Labels untereinander an

    // preview = lv_label_create(preview_cont, NULL);
    // lv_label_set_long_mode(preview, LV_LABEL_LONG_BREAK);
    // lv_obj_set_width(preview, 220);
    // // Den automatischen Zeilenumbruch für Fließtext aktivieren
    // lv_label_set_text(preview, "Bla, Aepfel, Birnen, Rapsölbutter, Zimt,
    // Ketchup");
    // // lv_label_set_text(preview, "Aepfel");
    // lv_obj_set_style_local_text_color(preview, LV_LABEL_PART_MAIN,
    // LV_STATE_DEFAULT, LV_COLOR_BLACK);
#endif

    // 2. KONTROLL-BUTTONS (Mittlere Zeile)
    btnRow = lv_btnmatrix_create(lv_scr_act(), nullptr);
    matBtnRow.init(btnRow);
    btnRow->user_data = this;
#if 1
    lv_obj_set_event_cb(btnRow, eventHandler);
#endif
    matBtnRow.mod.setDisabled(false);
    matBtnRow.filt.setDisabled(false);
    this->matBtnRow.setMap(this->matBtnRow.mSkip);
    setFilter(false); // damit labels geupdated werden und dadurch wird implizit
                      // der uncheckedIndex gefüllt.
    setFilter(true);
    // lv_btnmatrix_set_map(btnRow, modeMaps[mode]);
    // width and others will be set in updateLabels():
    // lv_btnmatrix_set_btn_width(btnRow, 0, 3);
    // lv_btnmatrix_set_btn_width(btnRow, 1, 3);
    // lv_btnmatrix_set_btn_width(btnRow, 2, 3);
    // lv_btnmatrix_set_btn_width(btnRow, 3, 3);
    {
        // buttonmatrixColor(btnRow, 255, 0, 0);
        buttonmatrixBgColor(btnRow, 20, 20, 20);
    }
    lv_obj_set_size(btnRow, 240, heightBtnRow);

    // {
    //     static lv_style_t matrix_style;
    //     lv_style_init(&matrix_style);

    //     // Setzt den inneren Button-Abstand auf 5 Pixel
    //     lv_style_set_pad_left(&matrix_style, LV_STATE_DEFAULT, 0);
    //     lv_style_set_pad_right(&matrix_style, LV_STATE_DEFAULT, 0);
    //     lv_style_set_pad_top(&matrix_style, LV_STATE_DEFAULT, 0);
    //     lv_style_set_pad_bottom(&matrix_style, LV_STATE_DEFAULT, 0);
    //     lv_style_set_pad_hor(&matrix_style, LV_STATE_DEFAULT, 1);
    //     lv_obj_add_style(btnRow, LV_BTNMATRIX_PART_BG, &matrix_style);
    // }
    // {
    //     static lv_style_t matrix_style;
    //     lv_style_init(&matrix_style);

    //     lv_style_set_radius(&matrix_style, LV_STATE_DEFAULT, 0);
    //     lv_obj_add_style(btnRow, LV_BTNMATRIX_PART_BTN, &matrix_style);
    // }

    // lv_obj_align(btnRow, lv_scr_act(), LV_ALIGN_IN_BOTTOM_LEFT, 0, 0);
    lv_obj_set_pos(btnRow, 0, yBtnRow);

    // log("heightPostview %d, heightDone %d, heightBtnRow %d, heightPreview %d,
    // sum of heights %d", heightPostview, heightDone, heightBtnRow,
    // heightPreview, heightPostview + heightDone + heightBtnRow +
    // heightPreview);

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

    // taskRefresh = lv_task_create(RefreshTaskCallback, 500, LV_TASK_PRIO_MID,
    // this);

    testModList();
}

void ShoppingList2::testModList()
{

    // BEGIN TEST
    lv_obj_clean(lv_scr_act());
    // auto cb = [this](lv_obj_t* obj, lv_event_t event) {
    //     log("cb of ModList called obj=%p, %d", obj, event);
    //     switch (event) {
    //         case LV_EVENT_PRESSED: {
    //             int pos = amountState.pos();
    //             if (!textWindow1.item(pos).empty()) {
    //                 amountState.nextPos();
    //                 modList.update(amountState, textWindow1, 0);
    //             }
    //             break;
    //         }
    //     }
    // };
    modList.create(lv_scr_act(), db, pr::ModListSubMode::CHECK);
    modList.addListener(this);
    // modList.update(amountState, textWindow1, 0);
    // END TEST
}

ShoppingList2::~ShoppingList2()
{
    modMenu.del();
    modList.del(); // muss unbedingt vor dem lv_obj_clean sein, da sonst dort
                   // tote Zeiger verwendet werden und del auf ungueltigen
                   // Pointer aufgerufen wird
    // lv_task_del(taskRefresh);
    // LVGL aufräumen, wenn der User die App über den physischen Button verlässt
    lv_obj_clean(lv_scr_act());
    log("Ende ~ShoppingList2");
}

void ShoppingList2::setFilter(bool newFilter)
{
    filter = newFilter;
    matBtnRow.filt.setCheckState(filter);
    while (filter && db.amountState.getAmount() == 0 &&
           !db.item(db.amountState.pos()).empty()) {
        db.amountState.nextPos();
    }
    updateLabels();
}

void ShoppingList2::OnButtonEvent(lv_obj_t* obj, lv_event_t event)
{
// log("OnButtonEvent");
#define TEXT(button) matBtnRow.button.getText()
    switch (event) {
        case LV_EVENT_PRESSED: {
            log("PRESSED");
            if (obj == this->btnRow) {
                log("BTN_ROW");
                const char* t = lv_btnmatrix_get_active_btn_text(btnRow);
                log("PRESSED on btnRow");

                if (t == TEXT(filt)) {
                    log("FILT");
                    setFilter(!filter);
                } else if (t == TEXT(mod)) {
                    log("MOD");
                    // this->mode = (this->mode + 1) % numModes;
                    // lv_btnmatrix_set_map(btnRow, modeMaps[mode]);
                    matBtnRow.setNextMap();
                    updateLabels();
                } else if (t == TEXT(bw)) {
                    log("pressed bw");
                    matBtnRow.bw.setCheckState(true);
                    updateLabels();
                } else if (t == TEXT(fw)) {
                    log("pressed fw");
                    matBtnRow.fw.setCheckState(true);
                    updateLabels();
                } else if (t == TEXT(mvBw)) {
                    log("pressed mvBw");
                    matBtnRow.mvBw.setCheckState(true);
                    updateLabels();
                } else if (t == TEXT(mvFw)) {
                    log("pressed mvFw");
                    matBtnRow.mvFw.setCheckState(true);
                    updateLabels();
                } else if (t == TEXT(skip)) {
                    log("pressed mvFw");
                    matBtnRow.skip.setCheckState(true);
                    updateLabels();
                }
            }
            break;
        }
        case LV_EVENT_RELEASED:
        // falls through
        case LV_EVENT_PRESS_LOST:
        // falls through
        case LV_EVENT_DRAG_BEGIN: {
            log("RELEASED");
            if (obj == this->btnRow) {
                log("BTN_ROW");
                matBtnRow.bw.setCheckState(false);
                matBtnRow.fw.setCheckState(false);
                matBtnRow.mvBw.setCheckState(false);
                matBtnRow.mvFw.setCheckState(false);
                matBtnRow.skip.setCheckState(false);
                updateLabels();
            }
            break;
        }

        case LV_EVENT_SHORT_CLICKED: {
            log("SHORT_CLICKED");
            if ((obj == this->done_btn)) {
                log("DONE: filter=%d", filter);
                // LFS_WARN("done clicked");

                if (matBtnRow.getMap() == &matBtnRow.mSkip) {
                    // skip mode
                    db.amountState.setAmount(0);
                    auto it = db.uncheckedIndex.remove(db.amountState.pos());

                    // Ähnlich wie bei ">":
                    if (filter) {
                        //   auto it = uncheckedIndex.find(amountState.pos());
                        if (it != db.uncheckedIndex.getEnd()) {
                            db.amountState.jump(it->pos);
                        } else {
                            int pos = db.amountState.pos();
                            std::string_view next = db.item(pos);
                            if (next.empty()) {
                                updateLabels();
                                return;
                            }
                            do {
                                db.amountState.nextPos();
                            } while (filter &&
                                     !db.item(db.amountState.pos()).empty() &&
                                     db.amountState.getAmount() == 0);
                        }

                    } else {

                        int pos = db.amountState.pos();
                        std::string_view next = db.item(pos);
                        if (next.empty()) {
                            updateLabels();
                            return;
                        }
                        db.amountState.nextPos();
                    }
                } else {
                    uint32_t amount = db.amountState.getAmount();
                    if (!db.item(db.amountState.pos()).empty()) {
                        if (filter) {
                            assert(amount > 0);
                            db.amountState.setAmount(amount - 1);
                            if (amount - 1 == 0) {
                                auto it = db.uncheckedIndex.remove(
                                    db.amountState.pos());
                                // Ähnlich wie bei ">":
                                if (filter) {
                                    //   auto it =
                                    //   uncheckedIndex.find(amountState.pos());
                                    if (it != db.uncheckedIndex.getEnd()) {
                                        db.amountState.jump(it->pos);
                                    } else {
                                        int pos = db.amountState.pos();
                                        std::string_view next = db.item(pos);
                                        if (next.empty()) {
                                            updateLabels();
                                            return;
                                        }
                                        do {
                                            db.amountState.nextPos();
                                        } while (filter &&
                                                 !db.item(db.amountState.pos())
                                                      .empty() &&
                                                 db.amountState.getAmount() ==
                                                     0);
                                    }

                                } else {

                                    int pos = db.amountState.pos();
                                    std::string_view next = db.item(pos);
                                    if (next.empty()) {
                                        updateLabels();
                                        return;
                                    }
                                    db.amountState.nextPos();
                                }
                            }
                        } else {
                            if (amount > 0) {
                                db.amountState.setAmount(amount - 1);
                                if (amount - 1 == 0) {
                                    db.uncheckedIndex.remove(
                                        db.amountState.pos());
                                }
                            } else {
                                db.amountState.nextPos();
                            }
                        }
                    } else {
                        LFS_WARN("done ignored");
                        return;
                    }
                }
                updateLabels();
            } else if (obj == btnRow) {
                log("BTN_ROW");
                const char* t = lv_btnmatrix_get_active_btn_text(btnRow);
                // log("click on btnRow");
                if (t == TEXT(bw)) {
                    log("BW");
                    // LFS_WARN("< clicked");
                    if (filter) {
                        auto it = db.uncheckedIndex.find(db.amountState.pos());
                        if (it != db.uncheckedIndex.getBegin()) {
                            --it;
                            db.item(it->pos);
                            db.amountState.jump(it->pos);
                            updateLabels();
                        }
                    } else {
                        if (db.amountState.pos() > 0) {
                            db.amountState.prevPos();
                            updateLabels();
                        } else {
                            LFS_WARN("< ignored");
                            return;
                        }
                    }
                } else if (t == TEXT(fw) || t == TEXT(skip)) {
                    log("FW");

                    // log("> clicked");
                    if (filter) {
                        // log("filter");
                        auto it = db.uncheckedIndex.find(db.amountState.pos());
                        if (it != db.uncheckedIndex.getEnd()) {
                            // log("not end");
                            if (it->end) {
                                // log("> ignored");
                            } else {
                                assert(!it->end);
                                ++it;
                                db.amountState.jump(it->pos);
                                updateLabels();
                            }
                        } else {
                            // log("end");
                            int pos = db.amountState.pos();
                            std::string_view next = db.item(pos);
                            if (next.empty()) {
                                // log("> ignored");
                                return;
                            }
                            do {
                                db.amountState.nextPos();
                            } while (filter &&
                                     !db.item(db.amountState.pos()).empty() &&
                                     db.amountState.getAmount() == 0);

                            updateLabels();
                        }

                    } else {
                        // log("!filter");

                        int pos = db.amountState.pos();
                        std::string_view next = db.item(pos);
                        if (next.empty()) {
                            // log("> ignored");
                            return;
                        }
                        db.amountState.nextPos();
                        updateLabels();
                    }
                } else if (t == TEXT(mvFw)) {
                    log("MV_FW");
                    if (false) {
                        // TODO begin test
                        testModList();
                        // TODO end test
                    } else {
                        // LFS_WARN("-> clicked");
                        int pos = db.amountState.pos();
                        std::string_view next = db.item(pos + 1);
                        if (next.empty()) {
                            LFS_WARN("-> ignored");
                            return;
                        }
                        db.swap(pos);
                        db.amountState.forward();

                        updateLabels();
                    }
                } else if (t == TEXT(mvBw)) {
                    log("MV_BW");
                    // LFS_WARN("<- clicked");
                    int pos = db.amountState.pos();
                    if (pos < 1 || db.item(pos).empty()) {
                        LFS_WARN("<- ignored");
                        return;
                    }
                    db.swap(pos - 1);
                    db.amountState.backward();
                    updateLabels();
                }
            }

            break;
        }
        case LV_EVENT_LONG_PRESSED: {
            log("LONG_PRESSED");
            if ((obj == this->done_btn)) {
                log("DONE");
                // LFS_WARN("done long pressed");
                uint32_t amount = db.amountState.getAmount();
                if (!db.item(db.amountState.pos()).empty()) {
                    db.amountState.setAmount(amount + 1);

                    updateLabels();
                } else {
                    LFS_WARN("done long press ignored");
                }
            }

            break;
        }
    }
#undef TEXT
}

bool ShoppingList2::OnTouchEvent(TouchEvents event)
{
    switch (event) {
        case TouchEvents::SwipeDown:
            if (modList.onSwipeDown())
                return true;
            return false;
        case TouchEvents::SwipeUp:
            if (modList.onSwipeUp())
                return true;
            return false;
        case TouchEvents::SwipeRight:
            if (modList.onSwipeRight())
                return true;
            return false;
        default:
            return false;
    }
}

void ShoppingList2::onEvent(pr::ModListEndEvent)
{
    // log("ShoppingList2::onModListEnd");
    modList.del();
    modMenu.create(lv_scr_act(), this);
}

void ShoppingList2::onEvent(pr::ModListSubMode subMode)
{
    modMenu.del();
    modList.create(lv_scr_act(), db, subMode);
    modList.addListener(this);
}

void ShoppingList2::updateLabels()
{
    log("updateLabels");
    auto pos = db.amountState.pos();
    // LFS_WARN("pos %d", pos);
    {
        pr::FixedStream ss(db.buf.data(), db.buf.size());
        if (!filter) {
            const int n = 2;
            int postviewBegin = std::max(0, pos - n);
            int postViewEnd = pos;
            for (int i = postviewBegin; i < postViewEnd; ++i) {
                if (i != postviewBegin) {
                    ss << ", ";
                }
                ss << (int) db.amountState.getAmount(i) << "x";
                formatItem(ss, db.item(i));
            }
        } else {
            // log("=================> filter");
            auto current = db.uncheckedIndex.find(pos);
            // log("end - current: %ld", uncheckedIndex.getEnd() - current);
            std::ptrdiff_t n = 2;
            auto begin =
                current - (std::min(n, current - db.uncheckedIndex.getBegin()));
            // log("iterating through uncheckedIndex");
            for (auto i = begin; i < current; ++i) {
                auto x = *i;
                // log("x.pos %d", x.pos);
                if (i != begin) {
                    ss << ", ";
                }
                ss << static_cast<int>(db.amountState.getAmount(x.pos)) << "x";
                formatItem(ss, db.item(x.pos));
            }
        }
        lv_label_set_text(postview, db.buf.data());
    }
    std::string_view item1(db.item(pos));
#define DISABLE(button) matBtnRow.button.setDisabled(true)
    matBtnRow.bw.setDisabled(false);
    matBtnRow.fw.setDisabled(false);
    matBtnRow.mvBw.setDisabled(false);
    matBtnRow.mvFw.setDisabled(false);
    matBtnRow.skip.setDisabled(false);
    // bool bwDisabled = false, fwDisabled = false, mvBwDisabled = false,
    // mvFwDisabled = false, skipDisabled = false;
    pr::FixedStream ss(db.buf.data(), db.buf.size());
    if (item1.empty()) {
        ss << "Fertig!";
        lv_btn_set_state(done_btn, LV_BTN_STATE_DISABLED);
        DISABLE(fw);
        // fwDisabled = true;
        DISABLE(skip);
        // skipDisabled = true;
    } else {
        ss << "(";
        ss << pos + 1;
        ss << ") ";
        ss << (int) db.amountState.getAmount() << "x";
        formatItem(ss, item1);

        // // std::string_view unit
        // ss << item;
        lv_btn_set_state(done_btn, LV_BTN_STATE_RELEASED);
    }

    lv_label_set_text(done_label, db.buf.data());
    // lv_obj_set_size(done_label, 240, heightDone);
    // lv_obj_set_pos(done_label, 0, 0);

    if (pos < 1 || (filter && db.uncheckedIndex.find(db.amountState.pos()) ==
                                  db.uncheckedIndex.getBegin())) {
        DISABLE(bw);
    }
    if (pos < 1) {
        DISABLE(mvBw);
    }
    if (db.item(pos).empty()) {
        DISABLE(mvBw);
    }
    std::string_view next = db.item(pos + 1);
    if (next.empty()) {
        DISABLE(mvFw);
    }

    matBtnRow.updateMapControls();

    // switch (mode) {
    // case 0: {
    //     lv_btnmatrix_ctrl_t btnRowCtrls[] = {btnState(bwDisabled, 1),
    //                                          btnState(false, 1),
    //                                          btnState(fwDisabled, 1)};
    //     lv_btnmatrix_set_ctrl_map(btnRow, btnRowCtrls);
    //     break;
    // }
    // case 1: {
    //     lv_btnmatrix_ctrl_t btnRowCtrls[] = {btnState(mvBwDisabled, 1),
    //                                          btnState(false, 1),
    //                                          btnState(mvFwDisabled, 1)};
    //     lv_btnmatrix_set_ctrl_map(btnRow, btnRowCtrls);
    //     break;
    // }
    // case 2: {
    //     lv_btnmatrix_ctrl_t btnRowCtrls[] = {btnState(false, 1),
    //                                          btnState(false, 1),
    //                                          btnState(skipDisabled, 1)};
    //     lv_btnmatrix_set_ctrl_map(btnRow, btnRowCtrls);
    //     break;
    // }
    // } // switch

    // lv_btnmatrix_set_btn_width(btnRow, 0, 3);
    // lv_btnmatrix_set_btn_width(btnRow, 1, 3);
    // lv_btnmatrix_set_btn_width(btnRow, 2, 3);
    // lv_btnmatrix_set_btn_width(btnRow, 3, 3);
    lv_btnmatrix_set_align(btnRow, LV_LABEL_ALIGN_LEFT);

    ss.clear();
    if (filter) {
        auto it = db.uncheckedIndex.find(pos);
        if (it != db.uncheckedIndex.getEnd()) {
            if (it->pos != pos) {
                ss << "OJE...";
            } else {
                bool first = true;
                for (++it; it != db.uncheckedIndex.getEnd() && !it->end; ++it) {
                    if (first) {
                        first = false;
                    } else {
                        ss << ", ";
                    }
                    item1 = db.item(it->pos);
                    ss << static_cast<int>(db.amountState.getAmount(it->pos))
                       << "x";
                    formatItem(ss, item1);
                }
            }
        }
    } else {
        for (int i = pos + 1; ss.size() < 18 * 4 && !item1.empty(); ++i) {
            item1 = db.item(i);
            // log("item1 '%.*s", SV_ARGS(item1));
            if (!item1.empty()) {
                if (i > pos + 1) {
                    ss << ", ";
                }
                ss << (int) db.amountState.getAmount(i) << "x";
                formatItem(ss, item1);
            }
        }
    }

    // log("preview string '%s'", buf.data());
    lv_label_set_text(preview, db.buf.data());
#undef DISABLE
}

void ShoppingList2::Refresh()
{
    log("ShoppingList2::Refresh()");
}
