#pragma once

#include "prInterfaces.h"
#include "prBitset128.h"

#include <bitset>

namespace pr {

class DoneState {
private:
    IFileFactory& fileFactory;
    Bitset128 _done;
    Bitset128 _skipped;
    int _pos;
    bool _fileExists;

public:
    DoneState(IFileFactory& fileFactory);
    void init();
    ~DoneState();
    bool fileExists() const { return _fileExists; }
    void done();
    void skip();
    void undo();
    void forward();
    void backward();
    int pos() const { return _pos; }
    bool isDone() const { return _done.test(_pos); }
    bool isDone(int pos) const { return _done.test(pos); }
    bool isSkipped() const { return _skipped.test(_pos); }
};


}

