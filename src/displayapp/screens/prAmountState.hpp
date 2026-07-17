#pragma once

#include "prUtils.hpp"
#include "prInterfaces.hpp"
#include "prBitsetU32.hpp"

namespace pr {

class AmountState {
private:

    IFileFactory& fileFactory;
    BitsetU32<u32PerAmounts> _amounts;
    int _pos;
    bool _fileExists;

public:

    AmountState(IFileFactory& fileFactory);
    AmountState(const AmountState&) = delete;
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
    void jump(int newPos);
};

}

