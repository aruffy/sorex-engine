#pragma once

namespace Sorex::Math
{
  template<typename T>
  class TNumericRange
  {
    static_assert(std::is_integral_v<T>,
                  __FILE__ " NumericRange: invalid value type");

public:
    using value_t = T;

    struct Iterator final
    {
      inline explicit Iterator(const value_t value) noexcept
        : _cur(value)
      {}

      Iterator& operator++() noexcept
      {
        ++_cur;
        return *this;
      }

      Iterator operator++(int) noexcept
      {
        Iterator other = this;
        ++_cur;
        return other;
      }

      inline value_t        operator*() const { return _cur; }
      inline const value_t* operator->() const { return &_cur; }

      inline bool operator==(const Iterator& other) const noexcept
      {
        return _cur == other._cur;
      }

      inline bool operator!=(const Iterator& other) const noexcept
      {
        return !(*this == other);
      }

      inline value_t GetValue() const { return _cur; }

  private:
      value_t _cur;
    };

public:
    inline TNumericRange(value_t first, value_t last) noexcept
      : _first(first)
      , _last(last)
    {
      SRX_CHECK(_first < _last);

      if (_first > _last)
        std::swap(_first, _last);
    }

    Iterator begin() const { return Iterator(_first); }
    Iterator end() const { return Iterator(_last); }

    inline bool Contains(value_t value) const
    {
      return (value >= _first) && (value < _last);
    }
    inline value_t GetCount() const { return _last - _first; }
    inline bool    IsEmpty() const { return _first >= _last; }

    inline value_t GetFirst() const { return _first; }
    inline value_t GetLast() const { return _last; }

private:
    value_t _first, _last;
  };
}
