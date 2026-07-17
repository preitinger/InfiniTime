#pragma once
#include <lvgl/lvgl.h>

namespace Pinetime
{
namespace Applications
{
namespace Widgets
{

class BaseIndicator
{
  public:
    BaseIndicator();
    virtual ~BaseIndicator() = default;
    void Create(uint16_t before, uint16_t visible, uint16_t after);
    void update(uint16_t before, uint16_t visible, uint16_t after);

  private:
    lv_point_t pageIndicatorBasePoints[2];
    lv_point_t pageIndicatorPoints[2];
    lv_obj_t* pageIndicatorBase;
    lv_obj_t* pageIndicator;
};

} // Widgets
} // Applications
} // Pinetime
