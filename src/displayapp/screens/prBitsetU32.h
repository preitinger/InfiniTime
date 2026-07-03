#pragma once

#include <array>
#include <cstdint>
#include <cassert>
#include <cstddef>

namespace pr {

template<size_t N>
class BitsetU32 {
public:
    std::array<uint32_t, N> data{}; // Initialisiert alle Bits auf 0

    static constexpr size_t size_bits = N * 32;

    // Setzt ein bestimmtes Bit auf 1
    void set(size_t bit) {
        assert(bit < size_bits);
        data[bit / 32] |= (1U << (bit % 32));
    }

    // Setzt ein bestimmtes Bit auf 0
    void unset(size_t bit) {
        assert(bit < size_bits);
        data[bit / 32] &= ~(1U << (bit % 32));
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
        if (bit >= size_bits) return false;
        return (data[bit / 32] & (1U << (bit % 32))) != 0;
    }

    // Liefert das rohe Byte-Array für LittleFS oder Bluetooth
    const uint8_t* getBytes() const {
        return reinterpret_cast<const uint8_t*>(data.data());
    }

    uint8_t* getBytes() {
        return reinterpret_cast<uint8_t*>(data.data());
    }

    // Schreibt einen uint-Wert mit einer bestimmten Bitlänge an einem Offset (optimiert)
    void setUint(size_t offset, size_t bitLength, uint32_t value) {
        assert(offset + bitLength <= size_bits);
        assert(bitLength > 0 && bitLength <= 32);

        size_t first_idx = offset / 32;
        size_t first_bit_pos = offset % 32;

        // Maske für die zu schreibenden Bits erstellen
        uint32_t mask = (bitLength == 32) ? 0xFFFFFFFF : (1U << bitLength) - 1;
        value &= mask;

        // Alten Wert an der Zielposition löschen
        data[first_idx] &= ~(mask << first_bit_pos);
        // Neuen Wert schreiben
        data[first_idx] |= (value << first_bit_pos);

        // Überprüfen, ob die Bits über die Grenze eines uint32_t hinausgehen
        size_t bits_in_first = 32 - first_bit_pos;
        if (bits_in_first < bitLength) {
            size_t second_idx = first_idx + 1;
            // Alten Wert im zweiten uint32_t löschen
            data[second_idx] &= ~(mask >> bits_in_first);
            // Neuen Wert schreiben
            data[second_idx] |= (value >> bits_in_first);
        }
    }

    // Liest einen uint-Wert mit einer bestimmten Bitlänge an einem Offset (optimiert)
    uint32_t getUint(size_t offset, size_t bitLength) const {
        assert(offset + bitLength <= size_bits);
        assert(bitLength > 0 && bitLength <= 32);

        size_t first_idx = offset / 32;
        size_t first_bit_pos = offset % 32;

        // Maske für die zu lesenden Bits erstellen
        uint32_t mask = (bitLength == 32) ? 0xFFFFFFFF : (1U << bitLength) - 1;

        // Bits aus dem ersten uint32_t lesen
        uint32_t result = (data[first_idx] >> first_bit_pos);

        // Überprüfen, ob die Bits über die Grenze eines uint32_t hinausgehen
        size_t bits_in_first = 32 - first_bit_pos;
        if (bits_in_first < bitLength) {
            size_t second_idx = first_idx + 1;
            // Bits aus dem zweiten uint32_t lesen und kombinieren
            result |= (data[second_idx] << bits_in_first);
        }

        return result & mask;
    }
};

using Bitset128 = BitsetU32<4>;


} // namespace pr
