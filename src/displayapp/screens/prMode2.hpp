#pragma once

#include "prFixedVector.hpp"
#include "prLog.hpp"
#include "prUtils.hpp"
#include <lvgl/lvgl.h>

#include <cassert>

namespace pr
{

// template <size_t MAX_NUM_BUTTONS, size_t NUM_MAPS>
class Matrix
{
  private:
    class Button
    {
      private:
        const char* text;
        lv_btnmatrix_ctrl_t ctrl;
        Button* next;

      public:
        Button(const char* text, Matrix& matrix);
        ~Button() = default;

        const char* getText() const
        {
            return text;
        }

#define DECLARE_FLAG(name, nameUp)                                             \
    bool get##name() const                                                     \
    {                                                                          \
        return getFlag(LV_BTNMATRIX_CTRL_##nameUp);                            \
    }                                                                          \
    void set##name(bool value)                                                 \
    {                                                                          \
        setFlag(LV_BTNMATRIX_CTRL_##nameUp, value);                            \
    }
        DECLARE_FLAG(Disabled, DISABLED)
        DECLARE_FLAG(Checkable, CHECKABLE)
        DECLARE_FLAG(CheckState, CHECK_STATE)
#undef DECLARE_FLAG

      private:
        bool getFlag(lv_btnmatrix_ctrl_t flag) const
        {
            return ctrl & flag;
        }

        void setFlag(lv_btnmatrix_ctrl_t flag, bool value)
        {
            if (value) {
                ctrl |= flag;
            } else {
                ctrl &= ~flag;
            }
        }

        friend class Matrix;
    };

    class BaseMap
    {
      private:
        BaseMap* next;

      public:
        virtual const char** getTexts() const = 0;
        virtual lv_btnmatrix_ctrl_t* prepCtrls() = 0;

        friend class Matrix;
    };

    template <size_t N>
    class Map : public BaseMap
    {
      private:
        FixedVector<lv_btnmatrix_ctrl_t, N> ctrls;
        FixedVector<const Button*, N> buttons;

      public:
        const char** texts;
        Map(const char** texts, Matrix& matrix);

        const char** getTexts() const override
        {
            return texts;
        }

        void setWidth(Button& b, int width);

      private:
        lv_btnmatrix_ctrl_t* prepCtrls() override;

        friend class Matrix;
    };

    lv_obj_t* obj;
    Button* firstButton;
    BaseMap* firstMap;
    BaseMap* lastMap;
    BaseMap* curMap;

  public:
    Matrix();

    void init(lv_obj_t* obj)
    {
        this->obj = obj;
    }

    Button mod, bw, fw, mvBw, mvFw, filt, skip;
    Map<4> mGo;
    Map<4> mMove;
    Map<4> mSkip;
    void setMap(BaseMap& map);

    BaseMap* getMap() const
    {
        return curMap;
    }

    void updateMapControls();
    void setNextMap();
    const Button* findButton(const char* text) const;

  private:
    void addButton(Button* button);
    void addMap(BaseMap* map);
};

inline bool notEmpty(const char* s)
{
    return *s;
};

inline bool isLf(const char* s)
{
    return *s == '\n' && s[1] == 0;
};

using MyMatrix = Matrix;

// template implementations

template <size_t N>
Matrix::Map<N>::Map(const char** texts, Matrix& matrix)
    : ctrls(), buttons(), texts(texts)
{
    for (auto text = texts; notEmpty(*text); ++text) {
        if (!isLf(*text)) {
            // zunaechst jeder Button disabled
            ctrls.push_back(LV_BTNMATRIX_CTRL_DISABLED | 1);
            buttons.push_back(matrix.findButton(*text));
        }
    }
    matrix.addMap(this);
}

template <size_t N>
lv_btnmatrix_ctrl_t* Matrix::Map<N>::prepCtrls()
{
    log("prepCtrls: buttons.size()=%d", (int) buttons.size());
    // disabled aus buttons aufsammeln
    assert(buttons.size() == ctrls.size());
    for (std::size_t i = 0; i < buttons.size(); ++i) {
        const Button* b = buttons[i];
        // nur eigene width behalten, rest von b->ctrl
        (ctrls[i] &= LV_BTNMATRIX_WIDTH_MASK) |= b->ctrl;
        dumpBtnMatCtrl("ctrl", ctrls[i]);
    }

    return ctrls.data();
}

template <size_t N>
void Matrix::Map<N>::setWidth(Button& b, int width)
{
    Button* pb = &b;
    for (size_t i = 0; i < this->buttons.size(); ++i) {
        if (buttons[i] == pb) {
            (ctrls[i] &= ~LV_BTNMATRIX_WIDTH_MASK) |=
                width & LV_BTNMATRIX_WIDTH_MASK;
        }
    }
}

} // namespace pr
