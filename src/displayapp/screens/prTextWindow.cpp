#include "prUtils.h"
#include "prTextWindow.h"
#include "prValidation.h"
#include "components/fs/FS.h"
#include "prLog.h"
#include "nrf_log.h"


namespace pr {


// static void mockFile(IFileFactory& ff) {
//     int code;
    
//     NRF_LOG_DEBUG("mockFile");
//     // lfs_file_t file;
//     IFilePtr f = ff.open(fileTxt, IFileFactory::WRONLY | IFileFactory::CREAT | IFileFactory::EXCL);
//     if (!f) {
//         log("/shoppingList.txt already existing, not mocking now");
//         return;
//     }
//     // code = fs.FileOpen(&file, fileTxt, LFS_O_WRONLY | LFS_O_CREAT | LFS_O_EXCL);
//     // LFS_DEBUG("open code %d", code);
//     // if (code < 0) return;

//     char* raw = new char[SIZE];
//     // BEGIN fake
//     int rawSize = snprintf(raw, SIZE, "Birnen\nBananen\nRapsoelbutter\nButter\nNescafe\n");
//     assert(rawSize > 0);
//     for (int i = 1; i <= 16; ++i) {
//         int code = snprintf(raw + rawSize, 256 - rawSize, "Item %d\n", i);
//         assert(code > 0);
//         rawSize += code;
//     }

//     code = f->write(raw, rawSize);
//     // code = fs.FileWrite(&file, raw, rawSize);
//     delete[] raw;

//     LFS_DEBUG("FileWrite code %d", code);
//     // code = fs.FileClose(&file);
//     // LFS_DEBUG("FileClose code %d", code);
//     f.reset();
// }

void TextWindow::init(pr::IFileFactory& fileFactory)
{
    if (!validateShoppingListFile(fileFactory, raw.data(), raw.size())) {
        log("/shoppingList.txt nicht valide!");
        return;
    }

    file1 = fileFactory.open(fileTxt, IFileFactory::RDWR | IFileFactory::CREAT);
    if (!file1) {
        log("/shoppingList.txt nicht gefunden");
        return;
    }
    int code;
    // if ((code = fs.FileOpen(&file, fileTxt, LFS_O_RDWR | LFS_O_CREAT)) != LFS_ERR_OK) {
    //     LFS_WARN("[ShoppingList] /shoppingList.txt nicht gefunden");
    //     return;
    // }

    // // BEGIN fake
    // rawSize = snprintf(raw, SIZE, "Birnen\nBananen\nRapsoelbutter\nButter\nNescafe\n");
    // LFS_DEBUG("rawSize=%d\n", rawSize);
    // // END fake

    file1->seek(this->offsetRaw);
    // code = fs.FileSeek(&file, this->offsetRaw);
    // if (code < 0) {
    //     LFS_ERROR("[ShoppingList] FileSeek fuer /shoppingList.txt mit code %d", code);
    //     return;
    // }
    code = file1->read(raw.data(), SIZE);
    // code = fs.FileRead(&file, raw, SIZE);
    // if (code < 0) {
    //     LFS_WARN("[ShoppingList] FileRead fuer /shoppingList.txt mit code %d", code);
    //     return;
    // }
    rawSize = code;
    LFS_DEBUG("init() rawSize %d", rawSize);
}

TextWindow::TextWindow()
    : file1(), raw(),
    rawSize(0), offsetRaw(0), pos(0), offsetPos(0), dirty(false)
{
    log("TextWindow()");
    // mockFile(fileFactory);
}

TextWindow::~TextWindow() {
    LFS_DEBUG("~TextWindow");
}

std::string_view TextWindow::item(int newPos) {
    // LFS_DEBUG("item %d", newPos);
    auto begin = offsetPos;
    auto end = begin;

    while (newPos > this->pos) {
        while (begin < rawSize && raw[begin] != '\n') {
            ++begin;
        }
        if (begin == rawSize) {
            if (rawSize == SIZE) {
                if (forward())
                    return item(newPos);
                else return "";
            }
            return "";
        }
        assert(raw[begin] == '\n');
        ++begin;
        ++pos;
        offsetPos = begin;
    }

    while (newPos < pos) {
        if (begin < 3) {
            if (!backward()) return "";
            return item(newPos);
        }
        assert(begin > 0);
        end = begin - 1;
        assert(raw[end] == '\n');
        begin = end - 1;
        while (begin > 0 && raw[begin] != '\n') {
            --begin;
        }
        if (raw[begin] != '\n') {
            if (offsetRaw > 0) {
                if (!backward()) {
                    return "";
                }
                return item(newPos);
            }
        }
        else {
            ++begin;
        }
        --pos;
        offsetPos = begin;
        // LFS_DEBUG("neues offsetPos bei rueckwaerts %d", offsetPos);
        if (pos == newPos) return snippet(begin, end);
    }

    if (pos == newPos) {
        end = begin;
        while (end < rawSize && raw[end] != '\n') {
            ++end;
        }
        if (end == rawSize) {
            if (!forward()) return "";
            return item(newPos);
        }
        std::string_view res = snippet(begin, end); //std::string_view( raw + begin,  raw + end);
        return res;
    }
    return "";
}

std::string_view TextWindow::snippet(int begin, int end) const {
    const char* data = reinterpret_cast<const char*>(raw.data());
    auto res = std::string_view(data + begin, data + end);
    // LFS_DEBUG("begin %d end%d", begin, end);
    // LFS_DEBUG("res: '%.*s'", (int)res.size(), res.data());
    return res;
}

bool TextWindow::forward() {
    // std::string dbg(snippet());
    // LFS_DEBUG("before forward ");
    if (!file1) return false;
    if (rawSize < SIZE) return false; // end of file reached
    if (offsetPos < HALF_SIZE) return false;
    offsetRaw += HALF_SIZE;
    offsetPos -= HALF_SIZE;

    int code;
    file1->seek(this->offsetRaw + HALF_SIZE);
    // int code = fs.FileSeek(&file, this->offsetRaw + HALF_SIZE);
    // if (code < 0) {
    //     LFS_ERROR("forward: FileSeek fuer /shoppingList.txt mit code %d", code);
    //     return false;
    // }
    memcpy(raw.data(), raw.data() + HALF_SIZE, HALF_SIZE);
    code = file1->read(raw.data() + HALF_SIZE, HALF_SIZE);
    // code = fs.FileRead(&file, raw + HALF_SIZE, HALF_SIZE);
    // if (code < 0) {
    //     LFS_WARN("forward: FileRead fuer /shoppingList.txt mit code %d", code);
    //     return false;
    // }
    rawSize = HALF_SIZE + code;
    // LFS_DEBUG("forward: rawSize %d", rawSize);


    // LFS_DEBUG("forward successful");
    return true;
}

bool TextWindow::backward() {
    // LFS_DEBUG("backward");
    if (!file1) return false;
    if (offsetRaw == 0) return false;
    int delta = std::min(offsetRaw, HALF_SIZE);
    if (offsetPos + delta > SIZE) return false;
    offsetRaw -= delta;
    offsetPos += delta;
    assert(offsetRaw >= 0);
    assert(offsetPos < SIZE);

    int code;
    file1->seek(this->offsetRaw);
    // int code = fs.FileSeek(&file, this->offsetRaw);
    // if (code < 0) {
    //     LFS_ERROR("backward: FileSeek fuer /shoppingList.txt mit code %d", code);
    //     return false;
    // }
    memcpy(raw.data() + HALF_SIZE, raw.data(), HALF_SIZE);
    code = file1->read(raw.data(), HALF_SIZE);
    // code = fs.FileRead(&file, raw, HALF_SIZE);
    // if (code < 0) {
    //     LFS_WARN("backward: FileRead fuer /shoppingList.txt mit code %d", code);
    //     return false;
    // }
    if (code != HALF_SIZE) {
        NRF_LOG_ERROR("code not HALF_SIZE, but %d", code);
    }
    assert(code == HALF_SIZE);
    rawSize = SIZE;
    // LFS_DEBUG("backward: offsetRaw %d, offsetPos %d, rawSize %d", offsetRaw, offsetPos, rawSize);


    // LFS_DEBUG("backward successful");
    return true;

}

void TextWindow::swap(int newPos) {
    int code;
    if (!file1) return;
    // LFS_DEBUG("swap newPos %d", newPos);
    // std::string sRaw(snippet(0, rawSize));
    // LFS_DEBUG("raw before swap: '%s'", sRaw.c_str());
    std::string cur(item(newPos));
    std::string next(item(newPos + 1));
    // LFS_DEBUG("cur '%s'", cur.c_str());
    // LFS_DEBUG("next '%s'", next.c_str());
    if (cur == "" || next == "") {
        log("TextWindow::swap ignored");
        return;
    }
    // Durch die vorangegangenen Aufrufe von item() und die Tatsache, dass 2 Zeilen garantiert immer kürzer sind als HALF_SIZE, 
    // ist garantiert, dass nun item(pos) und item(pos+1) komplett in raw liegen.
    item(newPos); // um offsetPos wieder auf cur zeigen zu lassen.
    // LFS_DEBUG("offsetPos %d, pos %d", offsetPos, pos);
    auto destp1 = raw.data() + offsetPos;
    memcpy(destp1, next.data(), next.size());
    raw[offsetPos + next.size()] = '\n';
    auto destp2 = raw.data() + offsetPos + next.size() + 1;
    memcpy(destp2, cur.data(), cur.size());
    // LFS_DEBUG("destp1 - raw: %d", (int) (destp1 - raw));
    // LFS_DEBUG("destp2 - raw: %d", (int)(destp2 - raw));
    assert(raw[offsetPos + next.size() + 1 + cur.size()] == '\n');
    raw[offsetPos + next.size() + 1 + cur.size()] = '\n';

    file1->seek(this->offsetRaw + offsetPos);
    // code = fs.FileSeek(&file, this->offsetRaw + offsetPos);
    // assert(code >= 0);

    code = file1->write(raw.data() + offsetPos, next.size() + 1 + cur.size());
    // code = fs.FileWrite(&file, raw + offsetPos, next.size() + 1 + cur.size());
    assert (code == (int) next.size() + 1 + (int) cur.size());
    if (code !=(int) next.size() + 1 + (int) cur.size()) {
        NRF_LOG_ERROR("IFile::write did return %d, but not %d", code, (int) next.size() + 1 + (int) cur.size());
    }


    // sRaw = snippet(0, rawSize);
    // LFS_DEBUG("raw after swap: '%s'", sRaw.c_str());

}

} // namespace pr
