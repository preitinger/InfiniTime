#pragma once
#include "prWidget.hpp"
#include <lvgl/lvgl.h>

namespace pr
{

template <typename Derived, typename PartEnum>
class HasStyle
{

  public:
    enum class State: lv_state_t {
        Default = LV_STATE_DEFAULT,
        Checked = LV_STATE_CHECKED,
        Focused = LV_STATE_FOCUSED,
        Edited = LV_STATE_EDITED,
        Hovered = LV_STATE_HOVERED,
        Pressed = LV_STATE_PRESSED,
        Disabled = LV_STATE_DISABLED,
    };

    void setBgColor(PartEnum part, State state, lv_color_t color)
    {
        lv_obj_set_style_local_bg_color(getObj1(),
                                        static_cast<uint8_t>(part),
                                        static_cast<lv_state_t>(state),
                                        color);
    }

    void setBgOpacity(PartEnum part, State state, lv_opa_t opacity)
    {
        lv_obj_set_style_local_bg_opa(getObj1(),
                                      static_cast<uint8_t>(part),
                                      static_cast<lv_state_t>(state),
                                      opacity);
    }

  private:
    lv_obj_t* getObj1()
    {
        return static_cast<Derived*>(this)->Widget::getObj();
    }
};

} // pr

