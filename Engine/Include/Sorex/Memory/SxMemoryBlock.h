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

#include <Sorex/SxCoreMinimal.h>

namespace Sorex
{
  template<typename T>
  class TMemoryBlock final
  {
    static_assert(std::is_integral_v<T>, "invalid type");

public:
    TMemoryBlock() = default;
    SRX_INLINE TMemoryBlock(T* data, size_t capacity) SRX_NOEXCEPT;

    /**
     * @brief Check if memory block is valid and has allocated memory.
     *
     * @return True is memory block has memory.
     */
    inline bool IsValid() const
    {
      return mData != nullptr || mCapacity >= sizeof(T);
    }

    /**
     * @brief Reset state of the object.
     *
     * @note: After call this function deallocation memory with this object is
     * impossible.
     *
     */
    inline void Nullify() { *this = TMemoryBlock(); }

    /**
     * @brief Return pointer to first byte of the buffer.
     *
     * @return pointer to inner buffer.
     */
    inline const T* GetData() const { return reinterpret_cast<T*>(mData); }
    inline T*       GetData() { return reinterpret_cast<T*>(mData); }

    /**
     * @brief Retrieve raw pointer to the buffer.
     *
     * @note: If you use raw pointer for writing also manage the size of data.
     *
     * @return  buffer pointer.
     */
    inline void* GetPtr() { return static_cast<void*>(mData); }

    /**
     * @brief Retrieve size of written data.
     *
     * @return amount of byte of written data.
     */
    inline size_t GetSize() const { return mSize; }

    /**
     * @brief Set size of data in the buffer.
     *
     * @param value - byte number of data;
     * @return Error:Ok if success, else error code.
     */
    inline bool SetSize(size_t value);

    inline size_t GetNum() const { return mSize / sizeof(T); }
    inline bool   SetNum(size_t n) { return SetSize(n * sizeof(T)); }

    /**
     * @brief Return capacity of the buffer in bytes.
     *
     * @return number of bytes.
     */
    inline size_t GetCapacity() const { return mCapacity; }

    /**
     * @bried Return true if in buffer no written data.
     *
     * @return True if buffer is empty, else False.
     */
    inline bool IsEmpty() const { return mSize == size_t(0); }
    inline void Clear() { mSize = 0u; }

    /**
     * @brief Fill the all buffer with the value.
     *
     * @param value to fill the buffer
     */
    void Fill(T value);

    /**
     * @brief Write data to buffer.
     *
     * Writing starts from begin of the buffer;
     *
     * @param data - pointer to data for writing.
     * @param size - amount of byte for writing.
     * @return - Error::Ok if success, else error code
     */
    EStatusCode        Write(const void* data, size_t size);
    inline EStatusCode Write(const T* data, size_t n)
    {
      return Write(static_cast<void*>(data), sizeof(T) * n);
    }

    /**
     * @brief Append data to the buffer.
     *
     * @param data - pointer to data for writing.
     * @param size - amount of byte for writing.
     * @return - Error::Ok if success, else error code
     */
    EStatusCode        Append(const void* data, size_t size);
    inline EStatusCode Append(const T* data, size_t n)
    {
      return Append(static_cast<void*>(data), sizeof(T) * n);
    }

    /**
     * @brief Add byte to the end of data in the buffer.
     *
     * @param - byte to write;
     * @return True if success, else false (no space)
     */
    bool PushBack(T value);

    /**
     * @brief Copy the written data from memory block to the buffer.
     *
     * @param buffer - storage for copying data;
     * @param size - size of buffer;
     * @param offset - offset from begin of data in memory block;
     * @return number of bytes that were copied;
     */
    size_t Read(void* buffer, size_t size, size_t offset = 0) const;
    size_t Read(T* buffer, size_t n, size_t offset = 0) const
    {
      return Read(static_cast<T>(buffer), sizeof(T) * n, offset * sizeof(T))
             / sizeof(T);
    }

private:
    byte*  mData     = nullptr;
    size_t mCapacity = 0;
    size_t mSize     = 0;
  };

  template<typename T>
  SRX_INLINE TMemoryBlock<T>::TMemoryBlock(T*     data,
                                           size_t capacity) SRX_NOEXCEPT
    : mData(data)
    , mCapacity(capacity)
    , mSize(0u)
  {}

  template<typename T>
  bool TMemoryBlock<T>::SetSize(size_t value)
  {
    if (value > mCapacity)
      return false;

    mSize = value;
    return true;
  }

  template<typename T>
  void TMemoryBlock<T>::Fill(T value)
  {
    const size_t maxNum = mCapacity / sizeof(T);
    std::fill_n(GetData(), maxNum, value);
    mSize = maxNum * sizeof(T);
  }

  template<typename T>
  EStatusCode TMemoryBlock<T>::Write(const void* data, size_t size)
  {
    SRX_CHECK(IsValid());

    if (data == nullptr || size == 0)
      return EStatusCode::Invalid_Argument;

    if (size > mCapacity)
      return EStatusCode::No_Space;

    std::copy_n(static_cast<const byte*>(data), size, mData);
    mSize = size;
    return EStatusCode::Ok;
  }

  template<typename T>
  EStatusCode TMemoryBlock<T>::Append(const void* data, size_t size)
  {
    SRX_CHECK(IsValid());

    if (data == nullptr || size == 0u)
      return EStatusCode::Invalid_Argument;

    const size_t avail = mCapacity - mSize;
    if (avail < size)
      return EStatusCode::No_Space;

    std::copy_n(static_cast<const byte*>(data), size, mData + mSize);
    mSize += size;
    return EStatusCode::Ok;
  }

  template<typename T>
  bool TMemoryBlock<T>::PushBack(T value)
  {
    const size_t avail = mCapacity - mSize;
    if (avail >= sizeof(T))
    {
      if constexpr (sizeof(T) == 1)
      {
        mData[mSize++] = value;
      }
      else
      {
        std::copy_n(reinterpret_cast<const byte*>(&value),
                    sizeof(value),
                    mData);
        mSize += sizeof(value);
      }

      return true;
    }

    return false;
  }

  template<typename T>
  size_t TMemoryBlock<T>::Read(void*  buffer,
                               size_t size,
                               size_t offset /* = 0 */) const
  {
    SRX_CHECK(IsValid());

    if (!buffer || !size)
      return 0;

    if (mSize == 0 || offset >= mSize)
      return 0;

    size = std::min(size, mSize - offset);
    std::copy_n(mData + offset, size, static_cast<byte*>(buffer));
    return size;
  }


  using MemoryBlock = TMemoryBlock<byte>;
}
