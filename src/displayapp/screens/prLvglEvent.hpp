#pragma once
#include <lvgl/lvgl.h>

namespace pr {

enum class LvglEventType : lv_event_t {
    Pressed = LV_EVENT_PRESSED,
    Pressing = LV_EVENT_PRESSING,
    PressLost = LV_EVENT_PRESS_LOST,
    ShortClicked = LV_EVENT_SHORT_CLICKED,
    LongPressed = LV_EVENT_LONG_PRESSED,
    LongPressedRepeat = LV_EVENT_LONG_PRESSED_REPEAT,
    Clicked = LV_EVENT_CLICKED,
    Released = LV_EVENT_RELEASED,
    DragBegin = LV_EVENT_DRAG_BEGIN,
    DragEnd = LV_EVENT_DRAG_END,
    DragThrowBegin =LV_EVENT_DRAG_THROW_BEGIN,
    Gesture = LV_EVENT_GESTURE,
    Key = LV_EVENT_KEY,
    Focused = LV_EVENT_FOCUSED,
    Defocused = LV_EVENT_DEFOCUSED,
    Leave = LV_EVENT_LEAVE,
    ValueChanged = LV_EVENT_VALUE_CHANGED,
    Insert = LV_EVENT_INSERT,
    Refresh = LV_EVENT_REFRESH,
    Apply = LV_EVENT_APPLY,
    Cancel = LV_EVENT_CANCEL,
    Delete = LV_EVENT_DELETE,
};

class Widget;

class LvglEvent
{
  public:
    LvglEventType type;
    Widget* widget;
};

void lvglEventBridge(lv_obj_t* obj, lv_event_t event);

} // pr
