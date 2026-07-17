#include "displayapp/widgets/PageIndicator.h"
#include "displayapp/InfiniTimeTheme.h"

using namespace Pinetime::Applications::Widgets;

PageIndicator::PageIndicator(uint8_t nCurrentScreen, uint8_t nScreens)
    : BaseIndicator(), nCurrentScreen {nCurrentScreen}, nScreens {nScreens}
{
}

void PageIndicator::Create()
{
    BaseIndicator::Create(nCurrentScreen, 1, nScreens - nCurrentScreen - 1);
    // pageIndicatorBasePoints[0].x = LV_HOR_RES - 1;
    // pageIndicatorBasePoints[0].y = 0;
    // pageIndicatorBasePoints[1].x = LV_HOR_RES - 1;
    // pageIndicatorBasePoints[1].y = LV_VER_RES;

    // pageIndicatorBase = lv_line_create(lv_scr_act(), nullptr);
    // lv_obj_set_style_local_line_width(pageIndicatorBase,
    //                                   LV_LINE_PART_MAIN,
    //                                   LV_STATE_DEFAULT,
    //                                   3);
    // lv_obj_set_style_local_line_color(pageIndicatorBase,
    //                                   LV_LINE_PART_MAIN,
    //                                   LV_STATE_DEFAULT,
    //                                   Colors::bgDark);
    // lv_line_set_points(pageIndicatorBase, pageIndicatorBasePoints, 2);

    // pageIndicator = lv_line_create(lv_scr_act(), nullptr);
    // lv_obj_set_style_local_line_width(pageIndicator,
    //                                   LV_LINE_PART_MAIN,
    //                                   LV_STATE_DEFAULT,
    //                                   3);
    // lv_obj_set_style_local_line_color(pageIndicator,
    //                                   LV_LINE_PART_MAIN,
    //                                   LV_STATE_DEFAULT,
    //                                   Colors::lightGray);
    // SetPageIndicatorPosition(nCurrentScreen);
}

void PageIndicator::SetPageIndicatorPosition(uint8_t position,
                                             uint8_t newScreens)
{
    if (newScreens > 0) nScreens = newScreens;
    update(position, 1, nScreens - position - 1);
    // if (newScreens > 0)
    //     nScreens = newScreens;
    // const int16_t indicatorSize = LV_VER_RES / nScreens;
    // const int16_t indicatorPos = indicatorSize * position;

    // pageIndicatorPoints[0].x = LV_HOR_RES - 1;
    // pageIndicatorPoints[0].y = indicatorPos;
    // pageIndicatorPoints[1].x = LV_HOR_RES - 1;
    // pageIndicatorPoints[1].y = indicatorPos + indicatorSize;

    // lv_line_set_points(pageIndicator, pageIndicatorPoints, 2);
}
