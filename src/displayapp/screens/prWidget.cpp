#include "prWidget.hpp"

#include <optional>

#include <cassert>

namespace pr
{

Widget::Widget() : eventSource(nullptr), obj(nullptr)
{
}

void Widget::create(lv_obj_t* obj, MyEventSource* eventSource)
{
    del();
    this->obj = obj;
    assert(!this->eventSource);
    this->eventSource = eventSource;
    if (eventSource) {
        obj->user_data = this;
        lv_obj_set_event_cb(obj, lvglEventBridge);
    }
}

void Widget::setPos(std::optional<Coord> x, std::optional<Coord> y)
{
    if (x || y) {
        // Falls ein Wert -1 ist, hole den aktuellen Wert direkt,
        // um set_pos nur EINMAL aufzurufen und Signale zu bündeln.
        lv_obj_set_pos(obj,
                       x ? *x : lv_obj_get_x(obj),
                       y ? *y : lv_obj_get_y(obj));
    }
}

void Widget::setSize(std::optional<Coord> w, std::optional<Coord> h)
{
    if (w || h) {
        // Gleiches Prinzip: Ein einziger Aufruf von set_size triggert
        // die schweren Layout-Berechnungen und Invalidierungen nur einmal.
        lv_obj_set_size(obj,
                        w ? *w : lv_obj_get_width(obj),
                        h ? *h : lv_obj_get_height(obj));
    }
}

void Widget::setRect(Coord x, Coord y, Coord w, Coord h)
{
    setPos(x, y);
    setSize(w, h);
}

bool Widget::hasChildren() const
{
    return lv_obj_get_child(obj, NULL) != nullptr;
}

void Widget::del()
{
    if (obj) {
        assert(!hasChildren());
        eventSource = nullptr;
        lv_obj_del(obj);
        obj = nullptr;
    }
}

Widget::~Widget()
{
    del();
}

} // pr
