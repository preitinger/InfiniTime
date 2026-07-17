#pragma once
#include <cstddef>
#include <cassert>

namespace pr
{

template <class T, std::size_t N>
class RingBuf
{
  public:
    RingBuf() = default;
    RingBuf(const RingBuf&) = delete;
    RingBuf(RingBuf&&) = delete;
    ~RingBuf() = default;

    T& push();
    T& popFront();

    bool isEmpty() const
    {
        return size == 0;
    }

    std::uint16_t getSize() const
    {
        return size;
    }

    T& popBack();

  private:
    std::array<T, N> buf;
    std::uint16_t first {};
    std::uint16_t size {};
};

// template implementations

template <class T, std::size_t N>
T& RingBuf<T, N>::push()
{
    T& t = buf[(first + size) % N];
    if (size < N)
        ++size;
    else
        first = (first + 1) % N;
    return t;
}

template <class T, std::size_t N>
T& RingBuf<T, N>::popFront()
{
    assert(size > 0);
    T& t = buf[first];
    --size;
    first = (first + 1) % N;
    return t;
}

template <class T, std::size_t N>
T& RingBuf<T, N>::popBack()
{
    // Sicherheitsnetz: Ein leeres Undo-System kann man nicht rückgängig machen
    assert(size > 0);

    // Das neueste Element liegt logisch an der Position: (first + size - 1)
    std::uint16_t lastIdx = (first + size - 1) % N;

    // Größe des Puffers schrumpft um eins, da das neueste Element entfernt wird
    --size;

    return buf[lastIdx];
}

} // pr
