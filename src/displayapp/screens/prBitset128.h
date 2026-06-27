#pragma once

#include <array>
#include <cstdint>
#include <cassert>

namespace pr {

class Bitset128 {
public:
    std::array<uint32_t, 4> data{}; // Initialisiert alle 128 Bits auf 0

    // Setzt ein bestimmtes Bit (0-127) auf 1
    void set(size_t bit) {
        assert(bit < 128);
        data[bit / 32] |= (1 << (bit % 32));
    }

    // Setzt ein bestimmtes Bit (0-127) auf 0
    void unset(size_t bit) {
        assert(bit < 128);
        data[bit / 32] &= ~(1 << (bit % 32));
    }

    void set(size_t bit, bool value) {
        if (value) {
            set(bit);
        } else {
            unset(bit);
        }
    }

    // Prüft, ob ein Bit gesetzt ist
    bool test(size_t bit) const {
        if (bit >= 128) return false;
        return (data[bit / 32] & (1 << (bit % 32))) != 0;
    }

    // Liefert das rohe Byte-Array für LittleFS oder Bluetooth
    const uint8_t* getBytes() const {
        return reinterpret_cast<const uint8_t*>(data.data());
    }

    uint8_t* getBytes() {
        return reinterpret_cast<uint8_t*>(data.data());
    }
};

} // namespace pr
