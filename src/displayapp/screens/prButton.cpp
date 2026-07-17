#include "prButton.hpp"
#include <cassert>

namespace pr
{

Button::Button() : Widget()
{
}

void Button::create(lv_obj_t* parent,
                    std::optional<Coord> x,
                    std::optional<Coord> y,
                    std::optional<Coord> w,
                    std::optional<Coord> h,
                    MyEventSource* eventSource,
                    const char* staticLabel)
{
    del();
    Widget::create(lv_btn_create(parent, nullptr), eventSource);
    setPos(x, y);
    setSize(w, h);

    if (staticLabel) {
        optionalLabel.create(*this);
        optionalLabel.setTextStatic(staticLabel);
        optionalLabel.alignOnButton(*this);
    }
}

void Button::create(lv_obj_t* parent, const ButtonInit& init)
{
    del();
    Widget::create(lv_btn_create(parent, nullptr), init.eventSource);

    setPos(init.x, init.y);
    setSize(init.w, init.h);
    if (init.staticLabel) {
        optionalLabel.create(*this);
        if (init.recolorLabel) {
            lv_label_set_recolor(optionalLabel.getObj(), *init.recolorLabel);
        }
        optionalLabel.setTextStatic(init.staticLabel);
        optionalLabel.alignOnButton(*this);
    }
    if (init.bgColorDefault) {
        setBgColor(Part::MAIN, HasStyle::State::Default, *init.bgColorDefault);
    }
}

void Button::del()
{
    optionalLabel.del();
    Widget::del();
}

Button::~Button()
{
}

}
