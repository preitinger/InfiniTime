#pragma once

#include "prInterfaces.h"
#include "prBitsetU32.h"

namespace pr {

class AmountState {
private:
    static constexpr int rawLen = 12 * sizeof(uint32_t);
    static constexpr int numItems = 128;
    static constexpr int bitsPerItem = 3;



    IFileFactory& fileFactory;
    BitsetU32<12> _amounts;
    int _pos;
    bool _fileExists;

public:
    static constexpr int expectedFileLen = sizeof(int) + rawLen;

    AmountState(IFileFactory& fileFactory);
    void init();
    ~AmountState();
    bool fileExists() const { return _fileExists; }
    
    void setAmount(uint32_t amount);
    uint32_t getAmount() const;
    uint32_t getAmount(int pos) const;


    void nextPos();
    void prevPos();
    void forward();
    void backward();
    int pos() const { return _pos; }
};

}

