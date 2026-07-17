#include "prMode2.hpp"
#include "prLog.hpp"

namespace pr
{

//
// BUTTON NAMES
//
static const char* tMod = "MOD";
static const char* tBw = "<";
static const char* tFw = ">";
static const char* tMvBw = "<-";
static const char* tMvFw = "->";
static const char* tFilt = "FILT";
static const char* tSkip = "SKIP";

//
// BUTTON MAPS
//
static const char* mapGo[] = {tMod, tBw, tFw, ""};
static const char* mapMove[] = {tMod, tMvBw, tMvFw, ""};
static const char* mapSkip[] = {tMod, tFilt, tSkip, ""};

Matrix::Button::Button(const char* text, Matrix& matrix)
    : text(text), ctrl(0), next(nullptr)
{
    matrix.addButton(this);
}

Matrix::Matrix()
    : obj(nullptr),
      firstButton(nullptr),
      firstMap(nullptr),
      lastMap(nullptr),
      curMap(nullptr),
      mod(tMod, *this),
      bw(tBw, *this),
      fw(tFw, *this),
      mvBw(tMvBw, *this),
      mvFw(tMvFw, *this),
      filt(tFilt, *this),
      skip(tSkip, *this),
      mGo(mapGo, *this),
      mMove(mapMove, *this),
      mSkip(mapSkip, *this)
{
    // mache ich lieber manuell:
    // filt.setCheckable(true);
}

static void dumpTexts(const char** texts)
{
    while (notEmpty(*texts)) {
        log("non-empty text: '%s'", *texts);
        ++texts;
    }
}

void Matrix::setMap(Matrix::BaseMap& map)
{
    curMap = &map;
    dumpTexts(map.getTexts());
    lv_btnmatrix_set_map(obj, map.getTexts());
    lv_btnmatrix_set_ctrl_map(obj, map.prepCtrls());
}

void Matrix::updateMapControls()
{
    if (curMap) {
        lv_btnmatrix_set_ctrl_map(obj, curMap->prepCtrls());
    }
}

void Matrix::setNextMap()
{
    setMap(*(curMap->next));
}

const Matrix::Button* Matrix::findButton(const char* text) const
{
    for (Button* b = firstButton; b != nullptr; b = b->next) {
        if (b->getText() == text) { // yes, it really is ==
            return b;
        }
    }

    return nullptr;
}

void Matrix::addButton(Button* button)
{
    button->next = firstButton;
    firstButton = button;
}

void Matrix::addMap(BaseMap* map)
{
    if (firstMap == nullptr) {
        lastMap = firstMap = map;
        map->next = map;
    } else {
        map->next = firstMap;
        lastMap->next = map;
        lastMap = map;
    }
}

void test()
{
    // const char* texts[] = {"bla", "blubb", ""};
    // Matrix::Map testMap(texts);

    Matrix matrix;
    matrix.setMap(matrix.mGo);
}

} // namespace pr
