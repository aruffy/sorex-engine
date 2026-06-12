/**************************************************************************/
/*                         This file is part of:                          */
/*                                SOREX                                   */
/*                 Simple OpenGL Rendering Engine eXtended                */
/**************************************************************************/
/* Copyright (c) 2022 Aleksandr Ershov (Ruffy).                           */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#pragma once

#include <Sorex/SxTypes.h>

#include "SxPoint.h"

namespace Sorex::Concept
{
  template<typename I, typename F>
  concept SafeConvIntToFlotingPoint =
    std::is_integral_v<I> and std::is_floating_point_v<F>
    and (std::numeric_limits<I>::max() <= std::numeric_limits<F>::max())
    and (std::numeric_limits<I>::min() >= std::numeric_limits<F>::min());
}

namespace Sorex::Math
{
  template<typename T>
  struct TIntegerSize
  {
private:
    static_assert(std::is_integral_v<T> and std::is_same_v<T, bool> == false,
                  "[TIntegerSize] Invalid value type");

    static constexpr T _kZero = T{ 0 };

public:
    using ValueType = T;

    T width  = _kZero;
    T height = _kZero;

    TIntegerSize() = default;
    SRX_INLINE TIntegerSize(const T w, const T h) SRX_NOEXCEPT
      : width(w)
      , height(h)
    {}

    TIntegerSize(const TIntegerSize& other) noexcept            = default;
    TIntegerSize& operator=(const TIntegerSize& other) noexcept = default;

    template<typename U>
      requires Concept::SafeIntegralConversion<U, T>
    // cppcheck-suppress noExplicitConstructor
    SRX_INLINE TIntegerSize(const TIntegerSize<U>& other) SRX_NOEXCEPT
      : width(static_cast<T>(other.width))
      , height(static_cast<T>(other.height))
    {}

    template<typename U>
      requires Concept::SafeIntegralConversion<U, T>
    SRX_INLINE TIntegerSize& operator=(const TIntegerSize<U>& other)
      SRX_NOEXCEPT
    {
      width  = static_cast<T>(other.width);
      height = static_cast<T>(other.height);
      return *this;
    }

    SRX_INLINE bool IsValid() const SRX_NOEXCEPT
    {
      return (width > _kZero && height > _kZero);
    }

    /**
     * @brief Set width of size. Ensures that width value will not be less zero;
     */
    template<typename U>
      requires Concept::SafeIntegralConversion<U, T>
    SRX_INLINE void SetWidth(const U value) SRX_NOEXCEPT
    {
      if constexpr (std::is_unsigned_v<T>)
        width = static_cast<T>(value);
      else
        width = std::max<T>(static_cast<T>(value), _kZero);
    }

    /**
     * @brief Set height of size. Ensures that height value will not be less
     * zero;
     */
    template<typename U>
      requires Concept::SafeIntegralConversion<U, T>
    SRX_INLINE void SetHeight(const U value) SRX_NOEXCEPT
    {
      if constexpr (std::is_unsigned_v<T>)
        height = static_cast<T>(value);
      else
        height = std::max(static_cast<T>(value), _kZero);
    }

    /**
     * @brief Set values of size. Ensures that values will not be less zero;
     */
    template<typename U>
      requires Concept::SafeIntegralConversion<U, T>
    SRX_INLINE void Set(const U w, const U h) SRX_NOEXCEPT
    {
      SetWidth(w);
      SetHeight(h);
    }

    /**
     * @brief Added values to width and height. Ensures that size values will
     * not be less zero;
     */
    template<typename U>
      requires Concept::SafeIntegralConversion<U, T>
    SRX_INLINE void Add(const U w, const U h) SRX_NOEXCEPT
    {
      if constexpr (std::is_unsigned_v<T>)
      {
        width += static_cast<T>(w);
        height += static_cast<T>(h);
      }
      else
      {
        width  = std::max<T>(width + w, _kZero);
        height = std::max<T>(height + h, _kZero);
      }
    }

    /**
     * @brief Added values to width and height. Ensures that size values will
     * not be less zero;
     */
    SRX_INLINE void Add(const TIntegerSize& other) SRX_NOEXCEPT
    {
      Add(other.width, other.height);
    }

    /**
     * @brief Added values to width and height. Ensures that size values will
     * not be less zero;
     */
    template<typename U>
      requires Concept::SafeIntegralConversion<U, T>
    SRX_INLINE void Add(const TIntegerSize<U>& other) SRX_NOEXCEPT
    {
      Add(other.width, other.height);
    }

    /**
     * @brief Subtract values from size.
     *
     * Ensures that size values will not be less zero;
     * If integer type is unsigned ensure that value will be zero in case when
     * args are more that values of size;
     *
     */
    template<typename U>
      requires Concept::SafeIntegralConversion<U, T>
    SRX_INLINE void Subtract(const U aW, const U aH) SRX_NOEXCEPT
    {
      const T w = static_cast<T>(aW);
      const T h = static_cast<T>(aH);
      if constexpr (std::is_unsigned_v<T>)
      {
        width  = width > w ? width - w : _kZero;
        height = height > h ? height - h : _kZero;
      }
      else
      {
        width  = std::max(width - w, _kZero);
        height = std::max(height - h, _kZero);
      }
    }

    /**
     * @brief Subtract values from size.
     *
     * Ensures that size values will not be less zero;
     * If integer type is unsigned ensure that value will be zero in case when
     * args are more that values of size;
     *
     */
    SRX_INLINE void Subtract(const TIntegerSize& other) SRX_NOEXCEPT
    {
      Subtract(other.width, other.height);
    }

    /**
     * @brief Subtract values from size.
     *
     * Ensures that size values will not be less zero;
     * If integer type is unsigned ensure that value will be zero in case when
     * args are more that values of size;
     *
     */
    template<typename U>
      requires Concept::SafeIntegralConversion<U, T>
    SRX_INLINE void Subtract(const TIntegerSize<U>& other) SRX_NOEXCEPT
    {
      Subtract(other.width, other.height);
    }

    SRX_INLINE bool operator==(const TIntegerSize& other) const SRX_NOEXCEPT
    {
      return (width == other.width && height == other.height);
    }

    SRX_INLINE bool operator!=(const TIntegerSize& other) const SRX_NOEXCEPT
    {
      return !(*this == other);
    }

    template<typename U>
      requires Concept::SameSignIntegral<U, T>
    SRX_INLINE bool operator==(const TIntegerSize<U>& other) const SRX_NOEXCEPT
    {
      if constexpr (sizeof(T) < sizeof(U))
        return (static_cast<U>(width) == other.width
                && static_cast<U>(height) == other.height);

      return (width == static_cast<T>(other.width)
              && height == static_cast<T>(other.height));
    }

    template<typename U>
      requires Concept::SameSignIntegral<U, T>
    SRX_INLINE bool operator!=(const TIntegerSize<U>& other) const SRX_NOEXCEPT
    {
      return !(*this == other);
    }
  };

  template<typename T>
  struct TFloatingSize
  {
private:
    static_assert(std::is_floating_point_v<T>,
                  "[TFloatingSize] Invalid value type");

    static constexpr T _kZero = T{ 0.f };

public:
    using ValueType = T;

    T width  = _kZero;
    T height = _kZero;

    TFloatingSize() = default;
    SRX_INLINE TFloatingSize(const T w, const T h) SRX_NOEXCEPT
      : width(w)
      , height(h)
    {}

    template<typename U>
      requires Concept::SafeConvIntToFlotingPoint<U, T>
    // cppcheck-suppress noExplicitConstructor
    SRX_INLINE TFloatingSize(const TIntegerSize<U>& size) SRX_NOEXCEPT
      : width(static_cast<T>(size.width))
      , height(static_cast<T>(size.height))
    {}

    SRX_INLINE bool IsValid() const SRX_NOEXCEPT
    {
      return (width > _kZero && height > _kZero);
    }

    /**
     * @brief Set width of size. Ensures that width value will not be less zero;
     */
    SRX_INLINE void SetWidth(const T value) SRX_NOEXCEPT
    {
      width = std::max<T>(value, _kZero);
    }

    /**
     * @brief Set height of size. Ensures that height value will not be less
     * zero;
     */
    SRX_INLINE void SetHeight(const T value) SRX_NOEXCEPT
    {
      height = std::max<T>(value, _kZero);
    }

    /**
     * @brief Set values of size. Ensures that values will not be less zero;
     */
    SRX_INLINE void Set(const T w, const T h) SRX_NOEXCEPT
    {
      SetWidth(w);
      SetHeight(h);
    }

    /**
     * @brief Added values to width and height. Ensures that size values will
     * not be less zero;
     */
    SRX_INLINE void Add(const T w, const T h) SRX_NOEXCEPT
    {
      width  = std::max(width + w, _kZero);
      height = std::max(height + h, _kZero);
    }

    /**
     * @brief Added values to width and height. Ensures that size values will
     * not be less zero;
     */
    SRX_INLINE void Add(const TFloatingSize& other) SRX_NOEXCEPT
    {
      Add(other.width, other.height);
    }

    /**
     * @brief Subtract values from size.
     *
     * Ensures that size values will not be less zero;
     * If integer type is unsigned ensure that value will be zero in case when
     * args are more that values of size;
     *
     */
    SRX_INLINE void Subtract(const T w, const T h) SRX_NOEXCEPT
    {
      width  = std::max(width - w, _kZero);
      height = std::max(height - h, _kZero);
    }

    template<std::integral Int>
      requires std::is_signed_v<Int>
    void Trunc(TIntegerSize<Int>& size) const SRX_NOEXCEPT
    {
      if constexpr (std::is_same_v<T, float>)
      {
        size.width  = static_cast<Int>(truncf(width));
        size.height = static_cast<Int>(truncf(height));
      }
      else
      {
        size.width  = static_cast<Int>(trunc(width));
        size.height = static_cast<Int>(trunc(height));
      }
    }

    SRX_INLINE void Trunc() SRX_NOEXCEPT
    {
      if constexpr (std::is_same_v<T, float>)
      {
        width  = truncf(width);
        height = truncf(height);
      }
      else
      {
        width  = trunc(width);
        height = trunc(height);
      }
    }

    template<std::integral Int>
      requires std::is_signed_v<Int>
    void Round(TIntegerSize<Int>& size) const SRX_NOEXCEPT
    {
      if constexpr (std::is_same_v<T, float>)
      {
        size.width  = static_cast<Int>(roundf(width));
        size.height = static_cast<Int>(roundf(height));
      }
      else
      {
        size.width  = static_cast<Int>(round(width));
        size.height = static_cast<Int>(round(height));
      }
    }

    SRX_INLINE void Round() SRX_NOEXCEPT
    {
      if constexpr (std::is_same_v<T, float>)
      {
        width  = roundf(width);
        height = roundf(height);
      }
      else
      {
        width  = round(width);
        height = round(height);
      }
    }

    template<std::integral Int>
      requires std::is_signed_v<Int>
    void Floor(TIntegerSize<Int>& size) const SRX_NOEXCEPT
    {
      if constexpr (std::is_same_v<T, float>)
      {
        size.width  = static_cast<Int>(floorf(width));
        size.height = static_cast<Int>(floorf(height));
      }
      else
      {
        size.width  = static_cast<Int>(floor(width));
        size.height = static_cast<Int>(floor(height));
      }
    }

    SRX_INLINE void Floor() SRX_NOEXCEPT
    {
      if constexpr (std::is_same_v<T, float>)
      {
        width  = floorf(width);
        height = floorf(height);
      }
      else
      {
        width  = floor(width);
        height = floor(height);
      }
    }

    template<std::integral Int>
      requires std::is_signed_v<Int>
    void Ceil(TIntegerSize<int32>& size) const SRX_NOEXCEPT
    {
      if constexpr (std::is_same_v<T, float>)
      {
        size.width  = static_cast<Int>(ceilf(width));
        size.height = static_cast<Int>(ceilf(height));
      }
      else
      {
        size.width  = static_cast<Int>(ceil(width));
        size.height = static_cast<Int>(ceil(height));
      }
    }

    SRX_INLINE void Ceil() SRX_NOEXCEPT
    {
      if constexpr (std::is_same_v<T, float>)
      {
        width  = ceilf(width);
        height = ceilf(height);
      }
      else
      {
        width  = ceil(width);
        height = ceil(height);
      }
    }

    SRX_INLINE bool IsEqual(const TFloatingSize& other) const SRX_NOEXCEPT
    {
      return (Math::IsEqual(width, other.width)
              && Math::IsEqual(height, other.height));
    }

    SRX_INLINE void Swap(TFloatingSize& other) SRX_NOEXCEPT
    {
      ValueType t;
      t           = other.width;
      other.width = width;
      width       = t;

      t            = other.height;
      other.height = height;
      height       = t;
    }

    static void Swap(TFloatingSize& a, TFloatingSize& b) SRX_NOEXCEPT
    {
      ValueType t;
      t       = a.width;
      a.width = b.width;
      b.width = t;

      t        = a.height;
      a.height = b.height;
      b.height = t;
    }

    /**
     * @brief Subtract values from size.
     *
     * Ensures that size values will not be less zero;
     * If integer type is unsigned ensure that value will be zero in case when
     * args are more that values of size;
     *
     */
    SRX_INLINE void Subtract(const TFloatingSize& other) SRX_NOEXCEPT
    {
      Subtract(other.width, other.height);
    }

    SRX_INLINE bool operator==(const TFloatingSize& other) const SRX_NOEXCEPT
    {
      return (width == other.width && height == other.height);
    }

    SRX_INLINE bool operator!=(const TFloatingSize& other) const SRX_NOEXCEPT
    {
      return !(*this == other);
    }
  };
}  // namespace

namespace Sorex
{
  using Size    = Math::TFloatingSize<scalar_t>;
  using SizeInt = Math::TIntegerSize<int>;
}

using SxSize = Sorex::Size;
