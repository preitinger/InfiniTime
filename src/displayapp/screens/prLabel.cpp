#include "prLabel.hpp"
#include "prButton.hpp"

#include <cassert>

namespace pr
{

Label::Label() : Widget()
{
}

void Label::create(lv_obj_t* parent,
                   std::optional<Coord> x,
                   std::optional<Coord> y,
                   std::optional<Coord> w,
                   std::optional<Coord> h)
{
    del();
    Widget::create(lv_label_create(parent, nullptr), nullptr);

    // Position mit minimalem Overhead setzen
    if (x || y) {
        // Falls ein Wert -1 ist, hole den aktuellen Wert direkt,
        // um set_pos nur EINMAL aufzurufen und Signale zu bündeln.
        lv_obj_set_pos(obj,
                       x ? *x : lv_obj_get_x(obj),
                       y ? *y : lv_obj_get_y(obj));
    }

    // Größe mit minimalem Overhead setzen
    if (w || h) {
        // Gleiches Prinzip: Ein einziger Aufruf von set_size triggert
        // die schweren Layout-Berechnungen und Invalidierungen nur einmal.
        lv_obj_set_size(obj,
                        w ? *w : lv_obj_get_width(obj),
                        h ? *h : lv_obj_get_height(obj));
    }
}

// void Label::create(lv_obj_t* parent, Coord x, Coord y, Coord w, Coord h)
// {
//     del();
//     Widget::create(lv_label_create(parent, nullptr));
//     if (x != -1 && y != -1) {
//         setPos(x, y);
//     }
//     if (w != -1 && h != -1) {
//         setSize(w, h);
//     }
// }

void Label::create(Widget& parent,
                   std::optional<Coord> x,
                   std::optional<Coord> y,
                   std::optional<Coord> w,
                   std::optional<Coord> h)
{
    this->create(parent.obj, x, y, w, h);
}

void Label::create(Widget& parent, const LabelInit& init)
{
    del();
    Widget::create(lv_label_create(parent.obj, nullptr), nullptr);
    assert(!init.longMode || !init.buttonAlignedTo);
    if (init.longMode) {
        setLongMode(*init.longMode);
    } else if (init.buttonAlignedTo) {
        alignOnButton(*init.buttonAlignedTo);
    }
    if (init.textStatic) {
        setTextStatic(init.textStatic);
    }
    if (init.recolor) {
        lv_label_set_recolor(obj, init.recolor);
    }
    setPos(init.x, init.y);
    setSize(init.w, init.h);
    if (init.bgOpa) {
        lv_obj_set_style_local_bg_opa(obj,
                                      LV_LABEL_PART_MAIN,
                                      LV_STATE_DEFAULT,
                                      *init.bgOpa);
    }
}

void Label::alignOnButton(Button& button)
{
    lv_label_set_long_mode(obj, LV_LABEL_LONG_EXPAND);
    lv_obj_align(obj, button.getObj(), LV_ALIGN_CENTER, 0, 0);
    // lv_obj_invalidate(obj);
}

void Label::setTextStatic(const char* buf)
{
    lv_label_set_text_static(obj, buf);
}

void Label::setLongMode(LongMode longMode)
{
    lv_label_set_long_mode(obj, to_underlying(longMode));
}

Label::~Label()
{
}

} // pr
