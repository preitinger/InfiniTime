#include "prUtils.hpp"
#include "prLog.hpp"
#include "prDoneState.hpp"
#include "displayapp/Controllers.h"
#include "components/fs/FS.h"

namespace pr {

constexpr int expectedFileLen = doneFileLen;

static bool lenOk(IFile& f) {
    f.seek(0);
    log("expectedFileLen: %d", expectedFileLen);
    uint8_t data[expectedFileLen];
    int len = (int) f.read(data, expectedFileLen);
    log("real len: %d", len);
    f.seek(0);
    return expectedFileLen == len;
}

DoneState::DoneState(IFileFactory& fileFactory) : fileFactory(fileFactory), _done(), _skipped(), _pos(0), _fileExists(false) {
}

void DoneState::init() {
    auto file = fileFactory.open(fileDone, IFileFactory::RDONLY);
    if (file && lenOk(*file)) {
        _fileExists = true;
        int pos;
        int n = file->read(&pos, sizeof(pos));
        assert(n == sizeof(pos));
        if (n != sizeof(pos)) {
            log("Falsche Dateigröße von /shoppingList-done.bin: %d statt %d", n, expectedFileLen);
        }
        log("read pos %d", pos);
        this->_pos = pos;
        n = file->read(&(_done.getBytes()[0]), rawLen);
        assert(n == rawLen);
        if (n != rawLen) {
            log("Falsche Dateigröße von /shoppingList-done.bin: %d statt %d", (int) sizeof(pos) + n, expectedFileLen);
            return;
        }
        n = file->read(_skipped.getBytes(), rawLen);
        log("n %d", n);
        assert(n == rawLen);
        if (n != rawLen) {
            log("Falsche Dateigröße von /shoppingList-done.bin: %d statt %d", (int) sizeof(pos) + rawLen + n, expectedFileLen);
            return;
        }
    }
    else {
        log("/shoppingList-done.bin nicht gefunden, done und skipped zurückgesetzt...");
    }
}
DoneState::~DoneState() {
    log("~DoneState");
    if (!_fileExists) return;
    auto file = fileFactory.open(fileDone, IFileFactory::WRONLY | IFileFactory::TRUNC);
    if (file) {
        int sum = 0;
        int n = file->write((uint8_t*)&_pos, sizeof(_pos));
        sum += n;
        assert(n == sizeof(_pos));
        n = file->write(&(_done.getBytes()[0]), 16);
        assert(n == 16);
        sum += n;
        n = file->write(&(_skipped.getBytes()[0]), 16);
        assert(n == 16);
        sum += n;
        assert(sum == expectedFileLen);
        file.reset();
    } else {
        log("=========> file null");
    }
}

void DoneState::done() {
    if (_pos < 128) {
        _done.set(_pos);
        // _done[_pos] = true;
        _skipped.unset(_pos);
        // _skipped[_pos] = false;
        ++_pos;
    }
}
void DoneState::skip() {
    if (_pos < 128) {
        // _done[_pos] = false;
        _done.unset(_pos);
        // _skipped[_pos] = true;
        _skipped.set(_pos);
        ++_pos;
    }
}

void DoneState::forward() {
    if (_pos + 1 < 128) {
        bool cur = _done.test(_pos);
        bool next = _done.test(_pos + 1);
        _done.set(_pos, next);
        _done.set(_pos + 1, cur);
        cur = _skipped.test(_pos);
        next = _skipped.test(_pos + 1);
        _skipped.set(_pos, next);
        _skipped.set(_pos + 1, cur);
        ++_pos;
    }
}

void DoneState::backward() {
    if (_pos > 0) {
        --_pos;
        bool cur = _done.test(_pos);
        bool next = _done.test(_pos + 1);
        _done.set(_pos, next);
        _done.set(_pos + 1, cur);
        cur = _skipped.test(_pos);
        next = _skipped.test(_pos + 1);
        _skipped.set(_pos, next);
        _skipped.set(_pos + 1, cur);
    }
}

void DoneState::undo() {
    if (_pos > 0) {
        --_pos;
        // _skipped.reset(_pos);
        // _done.reset(_pos);
    }
}

} // namespace pr
