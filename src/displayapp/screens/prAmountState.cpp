#include "prUtils.hpp"
#include "prLog.hpp"
#include "prAmountState.hpp"
#include "displayapp/Controllers.h"
#include "components/fs/FS.h"

namespace pr {

static bool lenOk(IFile& f) {
    f.seek(0);
    log("expectedFileLen: %d", AmountState::expectedFileLen);
    uint8_t data[AmountState::expectedFileLen];
    int len = (int)f.read(data, AmountState::expectedFileLen);
    log("real len: %d", len);
    f.seek(0);
    return AmountState::expectedFileLen == len;
}

AmountState::AmountState(IFileFactory& fileFactory) : fileFactory(fileFactory), _amounts(), _pos(0), _fileExists(false) {
}

void AmountState::init() {
    auto file = fileFactory.open(fileAmount, IFileFactory::RDONLY);
    if (file && lenOk(*file)) {
        _fileExists = true;
        int pos;
        int n = file->read(&pos, sizeof(pos));
        assert(n == sizeof(pos));
        if (n != sizeof(pos)) {
            log("Falsche Dateigröße von %s: %d statt %d", fileAmount, n, expectedFileLen);
        }
        log("read pos %d", pos);
        this->_pos = pos;
        n = file->read(_amounts.getBytes(), rawLen);
        assert(n == rawLen);
        if (n != rawLen) {
            log("Falsche Dateigröße von %s: %d statt %d", fileAmount, (int)sizeof(pos) + n, expectedFileLen);
            return;
        }
    }
    else {
        log("%s nicht gefunden, amounts zurückgesetzt...", fileAmount);
    }
}
AmountState::~AmountState() {
    log("~AmountState");
    if (!_fileExists) return;
    auto file = fileFactory.open(fileAmount, IFileFactory::WRONLY | IFileFactory::TRUNC);
    if (file) {
        int sum = 0;
        int n = file->write((uint8_t*)&_pos, sizeof(_pos));
        sum += n;
        assert(n == sizeof(_pos));
        n = file->write(_amounts.getBytes(), rawLen);
        assert(n == rawLen);
        sum += n;
        assert(sum == expectedFileLen);
        file.reset();
    }
    else {
        log("=========> file null");
    }
}

void AmountState::setAmount(uint32_t amount) {
    if (_pos < numItems) {
        _amounts.setUint(_pos * bitsPerItem, bitsPerItem, amount);
    }
}

uint32_t AmountState::getAmount() const {
    return getAmount(_pos);
}

uint32_t AmountState::getAmount(int pos) const {
    if (pos < numItems) {
        return _amounts.getUint(pos * bitsPerItem, bitsPerItem);
    }
    return 0;
}

void AmountState::forward() {
    if (_pos + 1 < numItems) {
        uint32_t currentAmount = getAmount(_pos);
        uint32_t nextAmount = getAmount(_pos + 1);
        _amounts.setUint(_pos * bitsPerItem, bitsPerItem, nextAmount);
        _amounts.setUint((_pos + 1) * bitsPerItem, bitsPerItem, currentAmount);
        ++_pos;
    }
}

void AmountState::backward() {
    if (_pos > 0) {
        --_pos;
        uint32_t currentAmount = getAmount(_pos);
        uint32_t nextAmount = getAmount(_pos + 1);
        _amounts.setUint(_pos * bitsPerItem, bitsPerItem, nextAmount);
        _amounts.setUint((_pos + 1) * bitsPerItem, bitsPerItem, currentAmount);
    }
}

void AmountState::nextPos() {
    ++_pos;
}

void AmountState::prevPos() {
    if (_pos > 0) {
        --_pos;
    }
}

} // namespace pr
