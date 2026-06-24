#include "prUtils.h"
#include "prLog.h"
#include "libs/littlefs/lfs.h"
#include <cassert>

namespace pr {

void writeExampleDone(IFilePtr& f, std::array<char, 256>& b) {
    memset(b.data(), 0, doneFileLen);
    [[maybe_unused]] int written = f->write(b.data(), doneFileLen);
    assert(written == doneFileLen);
}

void writeExample(IFilePtr& f) {
    std::array<char, 256> raw;
    writeExampleDone(f, raw);
    int rawSize = snprintf(raw.data(), raw.size(), "Birnen\nBananen\nRapsoelbutter\nButter\nNescafe\n");
    assert(rawSize > 0);
    for (int i = 1; i <= 16; ++i) {
        int code = snprintf(raw.data() + rawSize, 256 - rawSize, "Item %d\n", i);
        assert(code > 0);
        rawSize += code;
    }

    [[maybe_unused]] int code = f->write(raw.data(), rawSize);
    // code = fs.FileWrite(&file, raw.data(), rawSize);

    log("FileWrite code %d", code);
    // code = fs.FileClose(&file);
    // LFS_DEBUG("FileClose code %d", code);
}

void splitPacket(IFileFactory& factory) {
    IFilePtr fin = factory.open(fileExtIn, LFS_O_RDONLY);
    if (!fin) {
        log("Nicht gefunden: %s", fileExtIn);
        return;
    }
    IFilePtr fdone = factory.open(fileDone, LFS_O_WRONLY | LFS_O_TRUNC | LFS_O_CREAT);
    if (!fin) {
        log("Konnte nicht zum Schreiben oeffnen: %s", fileDone);
        return;
    }
    std::array<char, 256> b;
    // Die ersten `doneFileLen` Bytes nach `fileDone`.
    int len = fin->read(b.data(), doneFileLen);
    if (len != doneFileLen) {
        log("Nur %d Bytes aus %s gelesen", len, fileExtIn);
        return;
    }
    len = fdone->write(b.data(), doneFileLen);
    if (len != doneFileLen) {
        log("Nur %d Bytes in %s geschrieben", len, fileDone);
        return;
    }
    fdone.reset(); // close

    // Die restlichen nach `fileTxt`.
    IFilePtr ftxt = factory.open(fileTxt, LFS_O_WRONLY | LFS_O_TRUNC | LFS_O_CREAT);
    if (!ftxt) {
        log("Konnte nicht zum Schreiben oeffnen: %s", fileTxt);
        return;
    }

    do {
        len = fin->read(b.data(), b.size());
        int written = ftxt->write(b.data(), len);
        if (written != len) {
            log("Konnte nur %d statt %d nach %s schreiben", written, len, fileTxt);
            return;
        }
        log("%d nach %s geschrieben", written, fileTxt);
    } while (len == b.size());
}

template <std::size_t N>
void copyRest(IFilePtr& dst, IFilePtr& src, std::array<char, N> b, const char* nameDst, const char* nameSrc) {
    int len;

    do {
        len = src->read(b.data(), b.size());
        log("%d von %s gelesen", len, nameSrc);
        int written = dst->write(b.data(), len);
        if (written != len) {
            log("Konnte nur %d statt %d nach %s schreiben", written, len, nameDst);
            return;
        }
        log("%d nach %s geschrieben", written, nameDst);
    } while (len == b.size());
}

void concatPacket(IFileFactory& factory) {
    // Inhalt von fileDone und fileTxt konkateniert nach fileTmp schreiben
    std::array<char, 256> b;
    IFilePtr fdone = factory.open(fileDone, LFS_O_RDONLY);
    if (!fdone) {
        log("Konnte nicht zum Lesen öffnen: %s", fileDone);
        return;
    }

    IFilePtr ftmp = factory.open(fileTmp, LFS_O_WRONLY | LFS_O_TRUNC | LFS_O_CREAT);
    if (!ftmp) {
        log("Konnte nicht zum Schreiben oeffnen: %s", fileTmp);
        return;
    }

    copyRest(ftmp, fdone, b, fileTmp, fileDone);
    fdone.reset();
    IFilePtr ftxt = factory.open(fileTxt, LFS_O_RDONLY);
    if (!ftxt) {
        log("Konnte nicht zum Lesen öffnen: %s", fileTxt);
        return;
    }

    copyRest(ftmp, ftxt, b, fileTmp, fileTxt);

}


} // namespace pr
