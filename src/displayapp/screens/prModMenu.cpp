#include "prModMenu.hpp"

namespace pr
{

ModMenu::ModMenu(EventSource<LvglEvent>& lvglEventSource)
    : lvglEventSource(lvglEventSource),
      bClassic(),
      bCheck(),
      bStepBw(),
      bStepFw(),
      bMvBw(),
      bMvFw() /* ,
listener(nullptr) */
{
}

void ModMenu::create(lv_obj_t* scr, IEventListener<ModListSubMode>* listener)
{
    del();
    addListener(listener);
    lvglEventSource.addListener(this);
    // this->listener = listener;
    const int stepHeight = 57;
    const int gap = 4;
    int widthLeft = 120 - (gap >> 1);
    int widthRight = 240 - widthLeft - gap;
    int xRight = widthLeft + gap;
    int y = 0;
    bClassic.create(scr,
                    {
                        .x = 0,
                        .y = y,
                        .w = widthLeft,
                        .h = stepHeight,
                        .staticLabel = COL_CLASSIC "CLASSIC",
                        .recolorLabel = true,
                        .eventSource = &lvglEventSource,
                    });
    bCheck.create(scr,
                  ButtonInit {
                      .x = xRight,
                      .y = y,
                      .w = widthRight,
                      .h = stepHeight,
                      .staticLabel = COL_CHECK "CHECK",
                      .recolorLabel = true,
                      .eventSource = &lvglEventSource,
                  });
    y += stepHeight + gap;
    bStepBw.create(scr,
                   {
                       .x = 0,
                       .y = y,
                       .w = widthLeft,
                       .h = stepHeight,
                       .staticLabel = COL_STEP_BW "STEP BW",
                       .recolorLabel = true,
                       .eventSource = &lvglEventSource,
                   });
    bStepFw.create(scr,
                   {
                       .x = xRight,
                       .y = y,
                       .w = widthRight,
                       .h = stepHeight,
                       .staticLabel = COL_STEP_FW "STEP FW",
                       .recolorLabel = true,
                       .eventSource = &lvglEventSource,
                   });
    y += stepHeight + gap;
    bPageBw.create(scr,
                   {
                       .x = 0,
                       .y = y,
                       .w = widthLeft,
                       .h = stepHeight,
                       .staticLabel = COL_PAGE_BW "PAGE BW",
                       .recolorLabel = true,
                       .eventSource = &lvglEventSource,
                   });
    bPageFw.create(scr,
                   {
                       .x = xRight,
                       .y = y,
                       .w = widthRight,
                       .h = stepHeight,
                       .staticLabel = COL_PAGE_FW "PAGE FW",
                       .recolorLabel = true,
                       .eventSource = &lvglEventSource,
                   });
    y += stepHeight + gap;
    bMvBw.create(scr,
                 {
                     .x = 0,
                     .y = y,
                     .w = widthLeft,
                     .h = stepHeight,
                     .staticLabel = COL_MV_BW "SWAP BW",
                     .recolorLabel = true,
                     .eventSource = &lvglEventSource,
                 });
    bMvFw.create(scr,
                 {
                     .x = xRight,
                     .y = y,
                     .w = widthRight,
                     .h = stepHeight,
                     .staticLabel = COL_MV_FW "SWAP FW",
                     .recolorLabel = true,
                     .eventSource = &lvglEventSource,
                 });
    y += stepHeight + gap;
}

void ModMenu::onEvent(LvglEvent e)
{
    Widget* b = e.widget;
    switch (e.type) {
        case LvglEventType::ShortClicked:
            if (b == &bCheck) {
                fire(ModListSubMode::CHECK);
                // if (listener)
                //     listener->onModMenuCheck();
            } else if (b == &bStepBw) {
                fire(ModListSubMode::STEP_BW);
                // if (listener)
                // listener->onModMenuStepBw();
            } else if (b == &bStepFw) {
                fire(ModListSubMode::STEP_FW);
                // if (listener)
                //     listener->onModMenuStepFw();
            } else if (b == &bPageBw) {
                fire(ModListSubMode::PAGE_BW);
                // if (listener)
                //     listener->onModMenuPageBw();
            } else if (b == &bPageFw) {
                fire(ModListSubMode::PAGE_FW);
                // if (listener)
                //     listener->onModMenuPageFw();
            } else if (b == &bMvBw) {
                fire(ModListSubMode::MV_BW);
                // if (listener)
                //     listener->onModMenuMvBw();
            } else if (b == &bMvFw) {
                fire(ModListSubMode::MV_FW);
                // if (listener)
                //     listener->onModMenuMvFw();
            }
            break;
        default:
            break;
    }
}

void ModMenu::del()
{
    clearListeners();
    lvglEventSource.removeListener(this);

    // listener = nullptr;

    bMvFw.del();
    bMvBw.del();
    bPageFw.del();
    bPageBw.del();
    bStepFw.del();
    bStepBw.del();
    bCheck.del();
    bClassic.del();
}

ModMenu::~ModMenu()
{
    del(); // unbedingt notwendig
}

} // pr
