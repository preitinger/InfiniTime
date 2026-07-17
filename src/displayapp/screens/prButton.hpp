#pragma once

#include "prLvglEvent.hpp"
#include "prEvent.hpp"
#include "prHasStyle.hpp"
#include "prLabel.hpp"
#include "prWidget.hpp"
#include "prUtils.hpp"
#include <lvgl/lvgl.h>

#include <functional>
#include <optional>

namespace pr
{

class Button;

enum class ButtonPart : lv_btn_part_t { MAIN = LV_BTN_PART_MAIN };

struct ButtonInit {
    std::optional<Coord> x = std::nullopt;
    std::optional<Coord> y = std::nullopt;
    std::optional<Coord> w = std::nullopt;
    std::optional<Coord> h = std::nullopt;
    const char* staticLabel {};
    std::optional<bool> recolorLabel = false;
    std::optional<lv_color_t> bgColorDefault;
    EventSource<LvglEvent>* eventSource;
};

class Button : public Widget, public HasStyle<Button, ButtonPart>
{
    public:
    Label optionalLabel;

  public:
    using Part = ButtonPart;

    Button();
    /**
     * @deprecated
     */
    void create(lv_obj_t* parent,
                std::optional<Coord> x = std::nullopt,
                std::optional<Coord> y = std::nullopt,
                std::optional<Coord> w = std::nullopt,
                std::optional<Coord> h = std::nullopt,
                MyEventSource* eventSource = nullptr,
                const char* staticLabel = 0);

    void create(lv_obj_t* parent, const ButtonInit& init = {});

    void del() override;
    ~Button();
};

} // pr
