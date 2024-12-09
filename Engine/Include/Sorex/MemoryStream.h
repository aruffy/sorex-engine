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

#include "CoreMinimal.h"
#include "Stream.h"
#include "Utils/BytesUtils.h"

namespace Sorex
{
  template<typename T>
  class TReadOnlyDataStream final: public Stream
  {
    SRX_RTTI(TReadOnlyDataStream, Stream)

public:
    /**
     * @enum EParameters
     * @brief Enumeration for various parameter settings.
     *
     * This enum defines options for data handling and byte swapping.
     *
     * @var EParameters::Default
     * No data copy, enable integer bytes swapping for little endian system.
     *
     * @var EParameters::Integer_BytesSwap_Disable
     * No integer bytes swapping.
     *
     * @var EParameters::Integer_BytesSwap_Enable
     * Integers bytes swapping enabled.
     *
     * @var EParameters::Data_Copy
     * Indicates that data copying is enabled. Stream holds copy of the data.
     */
    enum class EParameters : uint8
    {
      Default                   = 0,
      Integer_BytesSwap_Disable = 0b01,
      Integer_BytesSwap_Enable  = 0b11,
      Data_Copy                 = 0b100,
      SRX_ENUM_BITMASK
    };

    static_assert(sizeof(T) == sizeof(byte), "Invalid data unit size");

public:
    using ValueType     = T;
    using ConstIterator = const ValueType*;

public:
    TReadOnlyDataStream() = default;
    TReadOnlyDataStream(ConstIterator it,
                        size_t        size,
                        EParameters flags = EParameters::Default) SRX_NOEXCEPT;
    TReadOnlyDataStream(ConstIterator first,
                        ConstIterator last,
                        EParameters flags = EParameters::Default) SRX_NOEXCEPT;

    virtual ~TReadOnlyDataStream() override;

    TReadOnlyDataStream(const TReadOnlyDataStream& other) SRX_NOEXCEPT;
    // cppcheck-suppress operatorEqToSelf
    TReadOnlyDataStream& operator=(const TReadOnlyDataStream& other)
      SRX_NOEXCEPT;

    TReadOnlyDataStream(TReadOnlyDataStream&& other) SRX_NOEXCEPT;
    TReadOnlyDataStream& operator=(TReadOnlyDataStream&& other) SRX_NOEXCEPT;

    SRX_INLINE ConstIterator begin() const { return mBegin; }
    SRX_INLINE ConstIterator end() const { return mEnd; }

    SRX_INLINE const ValueType* GetData() const { return mBegin; }

    /**
     * @brief Retrieve total size of items.
     *
     * @return total number of items.
     */
    SRX_INLINE size_t GetSize() const noexcept
    {
      return static_cast<size_t>(mEnd - mBegin);
    }

    SRX_INLINE const ValueType& operator[](const size_t i) const SRX_NOEXCEPT
    {
      return mBegin[i];
    }

    /**
     * @brief Retrieve value of item by index from the begin of data.
     *
     * @param i - index of the item
     * @param val - out value
     * @return True if item is into data and saved into val, else False.
     */
    bool Get(const size_t i, ValueType& val) const SRX_NOEXCEPT;

    /**
     * @brief Do step forward.
     *
     * @note: There is no any checking of stream state and range of step.
     *
     * @param step - step size.
     */
    SRX_INLINE void AdvanceUnsafe(const size_t step = 1) SRX_NOEXCEPT
    {
      SRX_CHECK(step < GetLength());
      mCurrent += step;
    }

    /**
     * @brief Do checked step forward if stream has more remained data than step
     * size.
     *
     * @param step - step size.
     * @return True if position of stream moved forward on step size, else
     * False.
     */
    bool Advance(const size_t step = 1) SRX_NOEXCEPT;

    /**
     * @brief Retrieve current value of the stream and advance forward.
     *
     * @note: There is no checking stream state or range of data.
     * Use Read(&value) if there is no guarantee that stream or range is valid.
     *
     * @return value of the item in current position of the stream.
     */
    SRX_INLINE T NextUnsafe() SRX_NOEXCEPT;
    SRX_INLINE TSpan<const ValueType> ReadUnsafe(const size_t length)
      SRX_NOEXCEPT;

    /**
     * @brief Read integer value from stream to the value.
     *
     * @note: There is no checking stream state or range of data.
     *  It swap bytes if the parameter is set
     *
     * @param value - storage for integer value
     */
    template<std::integral Int>
    void ReadIntUnsafe(Int& value) SRX_NOEXCEPT;

    // Interface Stream
    virtual ssize_t GetLength() const SRX_NOEXCEPT override
    {
      return static_cast<ssize_t>(mEnd - mCurrent);
    }

    virtual bool Check(const EAccessMode mode) const SRX_NOEXCEPT override
    {
      return (mode == EAccessMode::Read) && IsOpen();
    }

    virtual bool IsOpen() const SRX_NOEXCEPT override { return mBegin && mEnd; }
    virtual bool EndOfFile() SRX_NOEXCEPT override
    {
      return mCurrent == nullptr || mCurrent >= mEnd;
    }

    virtual ssize_t GetPosition() const SRX_NOEXCEPT override
    {
      SRX_CHECK(IsOpen());
      return static_cast<ssize_t>(mCurrent - mBegin);
    }

    virtual bool Seek(int32 pos, ESeekMode mode) SRX_NOEXCEPT override;
    virtual bool Peek(byte& value) SRX_NOEXCEPT override;

    virtual ssize_t Read(TSpan<byte> buffer, ssize_t length = SRX_UNKNOWN_SIZE)
      SRX_NOEXCEPT override;

    virtual ssize_t Write(TSpan<const byte> buffer) SRX_NOEXCEPT override;

    virtual bool Reset() SRX_NOEXCEPT override;

private:
    SRX_INLINE void Release() SRX_NOEXCEPT;

private:
    ConstIterator mBegin   = nullptr;
    ConstIterator mCurrent = nullptr;
    ConstIterator mEnd     = nullptr;

    ValueType*  mData    = nullptr;
    EParameters mOptions = EParameters::Default;
  };

  template<typename T>
  TReadOnlyDataStream<T>::TReadOnlyDataStream(
    ConstIterator it,
    size_t        size,
    EParameters   options /* = Default */) SRX_NOEXCEPT
    : mBegin(it)
    , mCurrent(it)
    , mEnd(it + size)
    , mData(Utils::CheckBitmask(options, EParameters::Data_Copy)
              ? new ValueType[size]
              : nullptr)
    , mOptions(options)
  {
    if (mData)
    {
      std::copy_n(it, size, mData);
      mBegin = mCurrent = mData;
      mEnd              = mData + size;
    }

    // NOTE: We check only if 0000000X - bit isn't set, then use the default
    if (!Utils::CheckBitmask(mOptions, EParameters::Integer_BytesSwap_Disable))
    {
      if (Utils::IsLittleEndian())
        mOptions |= EParameters::Integer_BytesSwap_Enable;
    }
  }

  template<typename T>
  TReadOnlyDataStream<T>::TReadOnlyDataStream(
    ConstIterator first,
    ConstIterator last,
    EParameters   options /* = Default */) SRX_NOEXCEPT
    : TReadOnlyDataStream(first, last - first, options)
  {}

  template<typename T>
  TReadOnlyDataStream<T>::~TReadOnlyDataStream()
  {
    Release();
  }

  template<typename T>
  TReadOnlyDataStream<T>::TReadOnlyDataStream(const TReadOnlyDataStream& other)
    SRX_NOEXCEPT
  {
    if (!other.mData)
    {
      Release();

      mBegin   = other.mBegin;
      mCurrent = other.mCurrent;
      mEnd     = other.mEnd;
    }
    else
    {
      const size_t size = other.GetSize();
      mData             = new ValueType[size];

      std::copy(other.mBegin, other.mEnd, mData);

      mBegin   = mData;
      mCurrent = mData + (other.mCurrent - other.mBegin);
      mEnd     = mData + size;
    }

    mOptions = other.mOptions;
    mStatus  = other.mStatus;
  }

  template<typename T>
  TReadOnlyDataStream<T>& TReadOnlyDataStream<T>::operator=(
    const TReadOnlyDataStream& other) SRX_NOEXCEPT
  {
    if (this == &other)
    {
      SRX_NOEXCEPT("operatorEqToSelf");
      return *this;
    }

    if (other.mData == nullptr)
    {
      Release();
      mBegin   = other.mBegin;
      mCurrent = other.mCurrent;
      mEnd     = other.mEnd;
    }
    else
    {
      const size_t size = other.GetSize();
      if (mData == nullptr || size > GetSize())
      {
        delete[] mData;
        mData = new ValueType[size];
      }

      std::copy(other.mBegin, other.mEnd, mData);

      mBegin   = mData;
      mCurrent = mData + (other.mCurrent - other.mBegin);
      mEnd     = mData + size;
    }

    mOptions = other.mOptions;
    mStatus  = other.mStatus;

    return *this;
  }

  template<typename T>
  TReadOnlyDataStream<T>::TReadOnlyDataStream(TReadOnlyDataStream&& other)
    SRX_NOEXCEPT
    : mBegin(other.mBegin)
    , mCurrent(other.mCurrent)
    , mEnd(other.mEnd)
    , mData(other.mData)
    , mOptions(other.mOptions)
  {
    other.mData = nullptr;
    mStatus     = std::move(other.mStatus);
  }

  template<typename T>
  TReadOnlyDataStream<T>& TReadOnlyDataStream<T>::operator=(
    TReadOnlyDataStream&& other) noexcept
  {
    if (this == &other)
    {
      SRX_NOEXCEPT("operatorEqToSelf");
      return *this;
    }

    Release();

    mBegin   = other.mBegin;
    mCurrent = other.mCurrent;
    mEnd     = other.mEnd;
    mData    = other.mData;
    mOptions = other.mOptions;
    mStatus  = std::move(other.mStatus);

    other.mData = nullptr;
    return *this;
  }

  template<typename T>
  bool TReadOnlyDataStream<T>::Get(const size_t i,
                                   ValueType&   val) const SRX_NOEXCEPT
  {
    if (i >= GetSize())
      return false;

    val = mBegin[i];
    return true;
  }

  template<typename T>
  bool TReadOnlyDataStream<T>::Advance(const size_t step /* = 1 */) SRX_NOEXCEPT
  {
    if (!IsOpen() || EndOfFile())
      return false;

    if (step >= GetLength())
      return false;

    mCurrent += step;
    return true;
  }

  template<typename T>
  SRX_INLINE T TReadOnlyDataStream<T>::NextUnsafe() SRX_NOEXCEPT
  {
    SRX_CHECK(IsOpen() && !EndOfFile());

    T tmp = mCurrent[0];
    ++mCurrent;
    return tmp;
  }

  template<typename T>
  SRX_INLINE TSpan<const typename TReadOnlyDataStream<T>::ValueType>
  TReadOnlyDataStream<T>::ReadUnsafe(const size_t length) SRX_NOEXCEPT
  {
    SRX_CHECK(IsOpen() && length < GetLength());

    const TSpan<const ValueType> result{ mCurrent, length };
    AdvanceUnsafe(length);

    return result;
  }

  template<typename T>
  template<std::integral Int>
  void TReadOnlyDataStream<T>::ReadIntUnsafe(Int& value) SRX_NOEXCEPT
  {
    if constexpr (sizeof(Int) == 1)
    {
      value = NextUnsafe();
      return;
    }

    constexpr size_t n = sizeof(value);
    std::copy_n(mCurrent, n, reinterpret_cast<ValueType*>(&value));

    AdvanceUnsafe(n);

    if (Utils::CheckBitmask(mOptions, EParameters::Integer_BytesSwap_Enable))
      value = Utils::SwapBytes(value);
  }

  template<typename T>
  bool TReadOnlyDataStream<T>::Seek(int32 pos, ESeekMode mode) SRX_NOEXCEPT
  {
    if (!IsOpen())
    {
      mStatus =
        SRX_STATUS_MSG(EStatusCode::Invalid_State, "Seek(): stream isn't open");
      return false;
    }

    const size_t size = GetSize();
    switch (mode)
    {
    case ESeekMode::Begin:
      break;
    case ESeekMode::Current:
      pos = GetPosition() + pos;
      break;
    case ESeekMode::End:
      pos = size + pos;
      break;
    default:
      SRX_NOENTRY("invalid seek mode");
      pos = -1;
      return false;
    }

    if (pos < 0 || (size_t)pos >= size)
    {
      mStatus = SRX_STATUS_MSG(EStatusCode::Out_Of_Range,
                               "Seek(): position is out of range");
      return false;
    }

    mCurrent = mBegin + pos;
    return true;
  }

  template<typename T>
  bool TReadOnlyDataStream<T>::Peek(byte& value) SRX_NOEXCEPT
  {
    if (EndOfFile())
      return false;

    value = mCurrent[0];
    return true;
  }

  template<typename T>
  ssize_t TReadOnlyDataStream<T>::Read(TSpan<byte> buffer,
                                       ssize_t     length) SRX_NOEXCEPT
  {
    SRX_CHECK(IsOpen());
    const bool bIsOpen = IsOpen();
    if (!bIsOpen || EndOfFile())
    {
      mStatus = SRX_STATUS_MSG(EStatusCode::Invalid_State,
                               "Read(): stream {}",
                               (bIsOpen ? "isn't open" : "eof"));
      return SRX_READ_ERROR;
    }

    const size_t toRead = (length == SRX_UNKNOWN_SIZE)
                            ? buffer.size()
                            : std::min<size_t>(buffer.size(), length);
    if (!toRead)
    {
      mStatus = SRX_STATUS_MSG(EStatusCode::Invalid_Argument,
                               "Read(): invalid length to read");
      return SRX_READ_ERROR;
    }

    const size_t n = std::min(toRead, static_cast<size_t>(GetLength()));
    std::copy_n(mCurrent, n, buffer.data());
    mCurrent += n;

    return static_cast<ssize_t>(n);
  }

  template<typename T>
  ssize_t TReadOnlyDataStream<T>::Write(TSpan<const byte> buffer) SRX_NOEXCEPT
  {
    mStatus =
      SRX_STATUS_MSG(EStatusCode::Not_Permitted, "Write(): read only stream");
    return SRX_WRITE_ERROR;
  }

  template<typename T>
  bool TReadOnlyDataStream<T>::Reset() SRX_NOEXCEPT
  {
    Stream::Reset();

    mCurrent = mBegin;
    return true;
  }

  template<typename T>
  SRX_INLINE void TReadOnlyDataStream<T>::Release() SRX_NOEXCEPT
  {
    if (mData)
    {
      delete[] mData;
      mData = nullptr;
    }
  }

  using ReadOnlyMemoryStream = TReadOnlyDataStream<byte>;
}  // namespace
