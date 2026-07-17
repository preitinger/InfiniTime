#include "prUtils.hpp"
#include "prLog.hpp"
#include "prAmountState.hpp"
#include "displayapp/Controllers.h"
#include "components/fs/FS.h"

namespace pr {

static bool lenOk(IFile& f) {
    f.seek(0);
    log("fileAmountLen: %d", fileAmountLen);
    uint8_t data[fileAmountLen];
    int len = (int)f.read(data, fileAmountLen);
    log("real len: %d", len);
    f.seek(0);
    return fileAmountLen == len;
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
            log("Falsche Dateigröße von %s: %d statt %d", fileAmount, n, fileAmountLen);
        }
        log("read pos %d", pos);
        this->_pos = pos;
        int expectedLen = fileAmountLen - sizeof(int);
        n = file->read(_amounts.getBytes(), expectedLen);
        assert(n == expectedLen);
        if (n != expectedLen) {
            log("Falsche Dateigröße von %s: %d statt %d", fileAmount, (int)sizeof(pos) + n, expectedLen);
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
        int expectedLen = fileAmountLen - sizeof(int);
        n = file->write(_amounts.getBytes(), expectedLen);
        assert(n == expectedLen);
        sum += n;
        assert(sum == fileAmountLen);
        file.reset();
    }
    else {
        log("=========> file null");
    }
}

void AmountState::setAmount(uint32_t amount) {
    if (_pos < maxItems) {
        _amounts.setUint(_pos * bitsPerAmount, bitsPerAmount, amount);
    }
}

uint32_t AmountState::getAmount() const {
    return getAmount(_pos);
}

uint32_t AmountState::getAmount(int pos) const {
    if (pos < maxItems) {
        return _amounts.getUint(pos * bitsPerAmount, bitsPerAmount);
    }
    return 0;
}

void AmountState::forward() {
    if (_pos + 1 < maxItems) {
        uint32_t currentAmount = getAmount(_pos);
        uint32_t nextAmount = getAmount(_pos + 1);
        _amounts.setUint(_pos * bitsPerAmount, bitsPerAmount, nextAmount);
        _amounts.setUint((_pos + 1) * bitsPerAmount, bitsPerAmount, currentAmount);
        ++_pos;
    }
}

void AmountState::backward() {
    if (_pos > 0) {
        --_pos;
        uint32_t currentAmount = getAmount(_pos);
        uint32_t nextAmount = getAmount(_pos + 1);
        _amounts.setUint(_pos * bitsPerAmount, bitsPerAmount, nextAmount);
        _amounts.setUint((_pos + 1) * bitsPerAmount, bitsPerAmount, currentAmount);
    }
}

void AmountState::jump(int newPos) {
    _pos = newPos;
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
