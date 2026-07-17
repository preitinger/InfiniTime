#pragma once

#include "prLvglEvent.hpp"
#include "prEvent.hpp"
#include "prUtils.hpp"

#include <lvgl/lvgl.h>

#include <optional>

namespace pr
{

class Widget
{
  public:
    using MyEventSource = EventSource<LvglEvent>;

  private:
    MyEventSource* eventSource;

  protected:
    lv_obj_t* obj;

  public:
    Widget();
    virtual void create(lv_obj_t* parent, MyEventSource*);
    virtual void setPos(std::optional<Coord> x = std::nullopt,
                        std::optional<Coord> y = std::nullopt);
    virtual void setSize(std::optional<Coord> w = std::nullopt,
                         std::optional<Coord> h = std::nullopt);
    virtual void setRect(Coord x, Coord y, Coord w, Coord h);
    virtual bool hasChildren() const;
    virtual void del();

    lv_obj_t* getObj() const
    {
        return obj;
    }

    virtual ~Widget();

    friend class Label;
    friend void lvglEventBridge(lv_obj_t* obj, lv_event_t event);
};
}