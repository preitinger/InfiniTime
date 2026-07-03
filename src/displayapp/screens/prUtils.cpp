#include "prUtils.h"
#include "prAmountState.h"
#include "prLog.h"
#include "libs/littlefs/lfs.h"

#include <memory>
#include <cassert>

namespace pr {

void writeExampleDone(IFilePtr& f, std::array<char, 256>& b) {
    memset(b.data(), 0, doneFileLen);
    [[maybe_unused]] int written = f->write(b.data(), doneFileLen);
    assert(written == doneFileLen);
}

void writeExampleId(IFilePtr& f, std::array<char, 256>& b) {
    for (int i = 0; i < 12; ++i) {
        b.data()[0] = i;
    }
    [[maybe_unused]] int written = f->write(b.data(), idLen);
    assert(written == idLen);
}

void writeExampleAmount(IFilePtr& f, std::array<char, 256>& b) {
    memset(b.data(), 0, pr::AmountState::expectedFileLen);
    auto amounts = std::make_unique<BitsetU32<12>>();
    for (int i = 0; i < 12 * 32 / 3; ++i) {
        amounts->setUint(i * 3, 3, 1);
    }

    b.data()[0] = 1;
    b.data()[1] = 0;
    b.data()[2] = 0;
    b.data()[3] = 0;
    [[maybe_unused]] int written = f->write(b.data(), 4);
    assert(written == 4);
    written = f->write(amounts->getBytes(), pr::AmountState::expectedFileLen - 4);
    assert(written == pr::AmountState::expectedFileLen - 4);
}

void writeExample(IFilePtr& f) {
    auto raw = std::make_unique<std::array<char, 256>>();
    writeExampleDone(f, *raw);
    int rawSize = snprintf(raw->data(), raw->size(), "123456789012345678901234567890\nBananen\nRapsoelbutter\nButter\nNescafe\n");
    assert(rawSize > 0);
    for (int i = 1; i <= 16; ++i) {
        int code = snprintf(raw->data() + rawSize, 256 - rawSize, "Item %d\n", i);
        assert(code > 0);
        rawSize += code;
    }

    [[maybe_unused]] int code = f->write(raw->data(), rawSize);
    // code = fs.FileWrite(&file, raw.data(), rawSize);

    log("FileWrite code %d", code);
    // code = fs.FileClose(&file);
    // LFS_DEBUG("FileClose code %d", code);
}

void writeExample2(IFilePtr& f) {
    auto raw = std::make_unique<std::array<char, 256>>();
    writeExampleAmount(f, *raw);
    int rawSize = snprintf(raw->data(), raw->size(), "Birnen\nBananen\nRapsoelbutter\nButter\nNescafe\n");
    assert(rawSize > 0);
    for (int i = 1; i <= 16; ++i) {
        int code = snprintf(raw->data() + rawSize, 256 - rawSize, "Item %d\n", i);
        assert(code > 0);
        rawSize += code;
    }

    [[maybe_unused]] int code = f->write(raw->data(), rawSize);
    // code = fs.FileWrite(&file, raw.data(), rawSize);

    log("FileWrite code %d", code);
    // code = fs.FileClose(&file);
    // LFS_DEBUG("FileClose code %d", code);
}

void writeExample3(IFilePtr& f) {
    auto raw = std::make_unique<std::array<char, 256>>();
    writeExampleId(f, *raw);
    writeExampleAmount(f, *raw);
    int rawSize = snprintf(raw->data(), raw->size(), "Birnen\t\tNicht matschig\nBananen\t\thalb matschig\nRapsölbutter\t\t\nButter\t200g\twenn billig\nNescafe\t\t\n");
    assert(rawSize > 0);
    for (int i = 1; i <= 16; ++i) {
        int code = snprintf(raw->data() + rawSize, 256 - rawSize, "Item %d\t\t\n", i);
        assert(code > 0);
        rawSize += code;
    }

    [[maybe_unused]] int code = f->write(raw->data(), rawSize);
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

void splitPacket2(IFileFactory& factory) {
    auto amountFileLen = AmountState::expectedFileLen;
    IFilePtr fin = factory.open(fileExtIn, LFS_O_RDONLY);
    if (!fin) {
        log("Nicht gefunden: %s", fileExtIn);
        return;
    }
    IFilePtr famount = factory.open(fileAmount, LFS_O_WRONLY | LFS_O_TRUNC | LFS_O_CREAT);
    if (!fin) {
        log("Konnte nicht zum Schreiben oeffnen: %s", fileAmount);
        return;
    }
    std::array<char, 256> b;
    // Die ersten `amountFileLen` Bytes nach `fileAmount`.
    int len = fin->read(b.data(), amountFileLen);
    if (len != amountFileLen) {
        log("Nur %d Bytes aus %s gelesen", len, fileExtIn);
        return;
    }
    len = famount->write(b.data(), amountFileLen);
    if (len != amountFileLen) {
        log("Nur %d Bytes in %s geschrieben", len, fileAmount);
        return;
    }
    famount.reset(); // close

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
void splitPacket3(IFileFactory& factory) {
    std::array<char, 256> b;
    auto amountFileLen = AmountState::expectedFileLen;

    IFilePtr fin = factory.open(fileExtIn, LFS_O_RDONLY);
    if (!fin) {
        log("Nicht gefunden: %s", fileExtIn);
        return;
    }

    IFilePtr fid = factory.open(fileId, LFS_O_WRONLY | LFS_O_TRUNC | LFS_O_CREAT);
    if (!fid) {
        log("Konnte nicht zum Schreiben oeffnen: %s", fileId);
        return;
    }
    int len = fin->read(b.data(), idLen);
    if (len != idLen) {
        log("Nur %d Bytes aus %s gelesen", len, fileExtIn);
        return;
    }
    len = fid->write(b.data(), idLen);
    if (len != idLen) {
        log("Nur %d statt %d Bytes in %s geschrieben", len, idLen, fileAmount);
        return;
    }
    fid.reset(); // close

    IFilePtr famount = factory.open(fileAmount, LFS_O_WRONLY | LFS_O_TRUNC | LFS_O_CREAT);
    if (!fin) {
        log("Konnte nicht zum Schreiben oeffnen: %s", fileAmount);
        return;
    }
    // Die ersten `amountFileLen` Bytes nach `fileAmount`.
    len = fin->read(b.data(), amountFileLen);
    if (len != amountFileLen) {
        log("Nur %d Bytes aus %s gelesen", len, fileExtIn);
        return;
    }
    len = famount->write(b.data(), amountFileLen);
    if (len != amountFileLen) {
        log("Nur %d Bytes in %s geschrieben", len, fileAmount);
        return;
    }
    famount.reset(); // close

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
bool copyRest(IFilePtr& dst, IFilePtr& src, std::array<char, N> b, [[maybe_unused]] const char* nameDst, [[maybe_unused]] const char* nameSrc) {
    int len;

    do {
        len = src->read(b.data(), b.size());
        log("%d von %s gelesen", len, nameSrc);
        int written = dst->write(b.data(), len);
        if (written != len) {
            log("Konnte nur %d statt %d nach %s schreiben", written, len, nameDst);
            return false;
        }
        log("%d nach %s geschrieben", written, nameDst);
    } while (len == b.size());
    return true;
}

bool concatPacket(IFileFactory& factory) {
    // Inhalt von fileDone und fileTxt konkateniert nach fileTmp schreiben
    std::array<char, 256> b;
    IFilePtr fdone = factory.open(fileDone, LFS_O_RDONLY);
    if (!fdone) {
        log("Konnte nicht zum Lesen öffnen: %s", fileDone);
        return false;
    }

    IFilePtr ftmp = factory.open(fileTmp, LFS_O_WRONLY | LFS_O_TRUNC | LFS_O_CREAT);
    if (!ftmp) {
        log("Konnte nicht zum Schreiben oeffnen: %s", fileTmp);
        return false;
    }

    copyRest(ftmp, fdone, b, fileTmp, fileDone);
    fdone.reset();
    IFilePtr ftxt = factory.open(fileTxt, LFS_O_RDONLY);
    if (!ftxt) {
        log("Konnte nicht zum Lesen öffnen: %s", fileTxt);
        return false;
    }

    return copyRest(ftmp, ftxt, b, fileTmp, fileTxt);
}

bool concatPacket2(IFileFactory& factory) {
    // Inhalt von fileAmount und fileTxt konkateniert nach fileTmp schreiben
    std::array<char, 256> b;
    IFilePtr fAmount = factory.open(fileAmount, LFS_O_RDONLY);
    if (!fAmount) {
        log("Konnte nicht zum Lesen öffnen: %s", fileAmount);
        return false;
    }

    IFilePtr ftmp = factory.open(fileTmp, LFS_O_WRONLY | LFS_O_TRUNC | LFS_O_CREAT);
    if (!ftmp) {
        log("Konnte nicht zum Schreiben oeffnen: %s", fileTmp);
        return false;
    }

    if (!copyRest(ftmp, fAmount, b, fileTmp, fileAmount)) return false;
    fAmount.reset();
    IFilePtr ftxt = factory.open(fileTxt, LFS_O_RDONLY);
    if (!ftxt) {
        log("Konnte nicht zum Lesen öffnen: %s", fileTxt);
        return false;
    }

    return copyRest(ftmp, ftxt, b, fileTmp, fileTxt);
}
bool concatPacket3(IFileFactory& factory) {
    // Inhalt von fileAmount und fileTxt konkateniert nach fileTmp schreiben
    std::array<char, 256> b;

    IFilePtr ftmp = factory.open(fileTmp, LFS_O_WRONLY | LFS_O_TRUNC | LFS_O_CREAT);
    if (!ftmp) {
        log("Konnte nicht zum Schreiben oeffnen: %s", fileTmp);
        return false;
    }

    IFilePtr fid = factory.open(fileId, LFS_O_RDONLY);
    if (!fid) {
        log("Konnte nicht zum Lesen öffnen: %s", fileId);
        return false;
    }
    if (!copyRest(ftmp, fid, b, fileTmp, fileId)) return false;
    fid.reset();

    IFilePtr fAmount = factory.open(fileAmount, LFS_O_RDONLY);
    if (!fAmount) {
        log("Konnte nicht zum Lesen öffnen: %s", fileAmount);
        return false;
    }


    if (!copyRest(ftmp, fAmount, b, fileTmp, fileAmount)) return false;
    fAmount.reset();
    IFilePtr ftxt = factory.open(fileTxt, LFS_O_RDONLY);
    if (!ftxt) {
        log("Konnte nicht zum Lesen öffnen: %s", fileTxt);
        return false;
    }

    return copyRest(ftmp, ftxt, b, fileTmp, fileTxt);
}


} // namespace pr
