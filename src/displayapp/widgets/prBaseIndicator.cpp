#include "prBaseIndicator.hpp"
#include "displayapp/screens/prUtils.hpp"
#include "displayapp/InfiniTimeTheme.h"

namespace Pinetime
{
namespace Applications
{
namespace Widgets
{

BaseIndicator::BaseIndicator() : pageIndicatorBasePoints()
{
}

void BaseIndicator::Create(uint16_t before, uint16_t visible, uint16_t after)
{
    pageIndicatorBasePoints[0].x = LV_HOR_RES - 1;
    pageIndicatorBasePoints[0].y = 0;
    pageIndicatorBasePoints[1].x = LV_HOR_RES - 1;
    pageIndicatorBasePoints[1].y = LV_VER_RES;

    pageIndicatorBase = lv_line_create(lv_scr_act(), nullptr);
    lv_obj_set_style_local_line_width(pageIndicatorBase,
                                      LV_LINE_PART_MAIN,
                                      LV_STATE_DEFAULT,
                                      3);
    lv_obj_set_style_local_line_color(pageIndicatorBase,
                                      LV_LINE_PART_MAIN,
                                      LV_STATE_DEFAULT,
                                      Colors::bgDark);
    lv_line_set_points(pageIndicatorBase, pageIndicatorBasePoints, 2);

    pageIndicator = lv_line_create(lv_scr_act(), nullptr);
    lv_obj_set_style_local_line_width(pageIndicator,
                                      LV_LINE_PART_MAIN,
                                      LV_STATE_DEFAULT,
                                      3);
    lv_obj_set_style_local_line_color(pageIndicator,
                                      LV_LINE_PART_MAIN,
                                      LV_STATE_DEFAULT,
                                      Colors::lightGray);
    update(before, visible, after);
}

void BaseIndicator::update(uint16_t before, uint16_t visible, uint16_t after)
{
    uint16_t sum = before + visible + after;
    pageIndicatorPoints[0].x = LV_HOR_RES - 1;
    // before / (before + visible + after) * LV_VER_RES
    pageIndicatorPoints[0].y = pr::roundedIntDiv(before * LV_VER_RES, sum);
    pageIndicatorPoints[1].x = LV_HOR_RES - 1;
    // (before + visible) / (before + visible + after) * LV_VER_RES
    // besser: LV_VER_RES - after / (before + visible + after) * LV_VER_RES
    pageIndicatorPoints[1].y =
        LV_VER_RES - pr::roundedIntDiv(after * LV_VER_RES, sum);

    lv_line_set_points(pageIndicator, pageIndicatorPoints, 2);
}

} // Widgets
} // Applications
} // Pinetime
