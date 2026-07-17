#pragma once

#include "prHasStyle.hpp"
#include "prWidget.hpp"
#include "prUtils.hpp"

#include <lvgl/lvgl.h>

#include <optional>

#include <cstdint>

namespace pr
{

class Button;

enum class LabelPart : lv_label_part_t { MAIN = LV_LABEL_PART_MAIN };

enum class LongMode : lv_label_long_mode_t {
    Expand = LV_LABEL_LONG_EXPAND,
    Break = LV_LABEL_LONG_BREAK,
    Dot = LV_LABEL_LONG_DOT,
    SRoll = LV_LABEL_LONG_SROLL,
    SRollCirc = LV_LABEL_LONG_SROLL_CIRC,
    Crop = LV_LABEL_LONG_CROP,
    opaDefault
};

struct LabelInit {
    std::optional<Coord> x = std::nullopt;
    std::optional<Coord> y = std::nullopt;
    std::optional<Coord> w = std::nullopt;
    std::optional<Coord> h = std::nullopt;
    std::optional<LongMode> longMode {};
    const char* textStatic = nullptr;
    Button* buttonAlignedTo = nullptr;
    bool recolor = false;
    std::optional<std::uint8_t> bgOpa {};
};

constexpr LabelInit testInit {};

class Label : public Widget, public HasStyle<Label, LabelPart>
{
  public:
    using Part = LabelPart;

    Label();

    void create(lv_obj_t* parent,
                std::optional<Coord> x = std::nullopt,
                std::optional<Coord> y = std::nullopt,
                std::optional<Coord> w = std::nullopt,
                std::optional<Coord> h = std::nullopt);
    void create(Widget& parent,
                std::optional<Coord> x = std::nullopt,
                std::optional<Coord> y = std::nullopt,
                std::optional<Coord> w = std::nullopt,
                std::optional<Coord> h = std::nullopt);
    void create(Widget& parent, const LabelInit& init);
    void alignOnButton(Button& button);

    void setTextStatic(const char* buf);

    template <typename... Args>
    void setTextFmt(const char* fmt, Args... args)
    {
        lv_label_set_text_fmt(obj, fmt, args...);
    }

    void setLongMode(LongMode longMode);
#define INLINE_SET_LONG_MODE(mode)                                             \
    void setLongMode##mode()                                                   \
    {                                                                          \
        setLongMode(LongMode::mode);                                           \
    }
    INLINE_SET_LONG_MODE(Expand)
    INLINE_SET_LONG_MODE(Break)
    INLINE_SET_LONG_MODE(Dot)
    INLINE_SET_LONG_MODE(SRoll)
    INLINE_SET_LONG_MODE(SRollCirc)
    INLINE_SET_LONG_MODE(Crop)
#undef INLINE_SET_LONG_MODE

    // void setTextFmt(const char* fmt, ...);
    ~Label();
};

} // pr
