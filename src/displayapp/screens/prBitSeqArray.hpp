#pragma once
#include "prBitsetU32.hpp"
#include <cstdint>
#include <cassert>

namespace pr
{

template <std::size_t BITS_PER_ENTRY, std::size_t NUM_ENTRIES>
class BitSeqArray
{
  public:
    static constexpr std::size_t neededBits = BITS_PER_ENTRY * NUM_ENTRIES;
    static constexpr std::size_t neededU32 = (neededBits + 31) / 32;

    void set(int index, unsigned int val);
    unsigned int get(int index) const;

  private:
    BitsetU32<neededU32> bs;
};

// template implementations

template <std::size_t BITS_PER_ENTRY, std::size_t NUM_ENTRIES>
void BitSeqArray<BITS_PER_ENTRY, NUM_ENTRIES>::set(int index, unsigned int val)
{
    assert(val < (1 <<BITS_PER_ENTRY));
    bs.setUint(BITS_PER_ENTRY * index, BITS_PER_ENTRY, val);
}

template <std::size_t BITS_PER_ENTRY, std::size_t NUM_ENTRIES>
unsigned int BitSeqArray<BITS_PER_ENTRY, NUM_ENTRIES>::get(int index) const
{
    return bs.getUint(BITS_PER_ENTRY * index, BITS_PER_ENTRY);
}

} // pr
