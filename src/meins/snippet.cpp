#include <lvgl/lvgl.h>

// Callbacks für die Buttons
static void done_click_cb(lv_obj_t * obj, lv_event_t event) {
    if(event == LV_EVENT_CLICKED) {
        // TODO: Sende Bluetooth-Signal für "Done" an das Smartphone
    }
}

static void skip_click_cb(lv_obj_t * obj, lv_event_t event) {
    if(event == LV_EVENT_CLICKED) {
        // TODO: Sende Bluetooth-Signal für "Skip"
    }
}

static void undo_click_cb(lv_obj_t * obj, lv_event_t event) {
    if(event == LV_EVENT_CLICKED) {
        // TODO: Sende Bluetooth-Signal für "Undo"
    }
}

void create_shopping_list_ui() {
    // 1. DER RIESEN-BUTTON (Aktuelles Item)
    lv_obj_t * done_btn = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_set_size(done_btn, 240, 110); // Halbe Bildschirmhöhe
    lv_obj_align(done_btn, NULL, LV_ALIGN_IN_TOP_MID, 0, 0);
    lv_obj_set_event_cb(done_btn, done_click_cb);
    
    // Text im Riesen-Button zentrieren
    lv_obj_t * done_label = lv_label_create(done_btn, NULL);
    lv_label_set_text(done_label, "1. Bananen");
    // Textumbruch aktivieren, falls das Wort zu lang ist
    lv_label_set_long_mode(done_label, LV_LABEL_LONG_BREAK); 
    lv_obj_set_width(done_label, 220);
    lv_obj_align(done_label, NULL, LV_ALIGN_CENTER, 0, 0);

    // 2. KONTROLL-BUTTONS (Mittlere Zeile)
    // Skip-Button (Links)
    lv_obj_t * skip_btn = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_set_size(skip_btn, 115, 40);
    lv_obj_align(skip_btn, done_btn, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5);
    lv_obj_set_event_cb(skip_btn, skip_click_cb);
    
    lv_obj_t * skip_label = lv_label_create(skip_btn, NULL);
    lv_label_set_text(skip_label, "SKIP");

    // Undo-Button (Rechts)
    lv_obj_t * undo_btn = lv_btn_create(lv_scr_act(), NULL);
    lv_obj_set_size(undo_btn, 115, 40);
    lv_obj_align(undo_btn, done_btn, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 5);
    lv_obj_set_event_cb(undo_btn, undo_click_cb);
    
    lv_obj_t * undo_label = lv_label_create(undo_btn, NULL);
    lv_label_set_text(undo_label, "UNDO");

    // 3. VORSCHAU-BEREICH (2 Spalten per LVGL Container / Grid)
    // Linke Spalte
    lv_obj_t * col_left = lv_cont_create(lv_scr_act(), NULL);
    lv_obj_set_size(col_left, 115, 75);
    lv_obj_align(col_left, skip_btn, LV_ALIGN_OUT_BOTTOM_LEFT, 0, 5);
    lv_cont_set_layout(col_left, LV_LAYOUT_COLUMN_LEFT); // Ordnet Labels untereinander an

    lv_obj_t * item2 = lv_label_create(col_left, NULL);
    lv_label_set_text(item2, "2. Milch");
    lv_obj_t * item4 = lv_label_create(col_left, NULL);
    lv_label_set_text(item4, "4. Brot");

    // Rechte Spalte
    lv_obj_t * col_right = lv_cont_create(lv_scr_act(), NULL);
    lv_obj_set_size(col_right, 115, 75);
    lv_obj_align(col_right, undo_btn, LV_ALIGN_OUT_BOTTOM_RIGHT, 0, 5);
    lv_cont_set_layout(col_right, LV_LAYOUT_COLUMN_LEFT);

    lv_obj_t * item3 = lv_label_create(col_right, NULL);
    lv_label_set_text(item3, "3. Eier");
    lv_obj_t * item5 = lv_label_create(col_right, NULL);
    lv_label_set_text(item5, "5. Kaffee");
}
