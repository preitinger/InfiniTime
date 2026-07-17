#include "prLvglEvent.hpp"
#include "prWidget.hpp"

namespace pr
{

void lvglEventBridge(lv_obj_t* obj, lv_event_t event)
{
    auto widget = static_cast<Widget*>(lv_obj_get_user_data(obj));
    if (widget->eventSource) {
        LvglEventType type = static_cast<LvglEventType>(event);
        widget->eventSource->fire(LvglEvent {type, widget});
    }
}

} // pr
