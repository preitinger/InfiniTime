#pragma once
#include <array>
#include <cstddef>
#include <stdexcept>

namespace pr
{

template <typename T, std::size_t Capacity>
class FixedVector
{
  private:
    std::array<T, Capacity> m_data {};
    std::size_t m_size = 0;

  public:
    // Die wichtigsten Vektor-Funktionen einfach nachbauen
    bool push_back(const T& value)
    {
        if (m_size >= Capacity)
            return false; // Embedded-freundlich: Fehlercode statt Exception!
        m_data[m_size++] = value;
        return true;
    }

    void pop_back()
    {
        if (m_size > 0)
            m_size--;
    }

    void clear()
    {
        m_size = 0;
    }

    T& operator[](std::size_t index)
    {
        return m_data[index];
    }

    const T& operator[](std::size_t index) const
    {
        return m_data[index];
    }

    std::size_t size() const
    {
        return m_size;
    }

    constexpr std::size_t capacity() const
    {
        return Capacity;
    }

    // Iteratoren für moderne for-Schleifen (range-based for)
    auto begin()
    {
        return m_data.begin();
    }

    auto end()
    {
        return m_data.begin() + m_size;
    }

    const T* data() const
    {
        return m_data.data();
    }

    T* data()
    {
        return m_data.data();
    }
};

}