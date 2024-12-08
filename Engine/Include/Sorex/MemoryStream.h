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

namespace Sorex
{
  template<typename T>
  class ReadOnlyDataStream final: public Stream
  {
    SRX_RTTI(ReadOnlyDataStream, Stream)

public:
    enum class EOption : uint8
    {
      Default                      = 0,
      Integer_Bytes_Swap_Custom    = (1 << 0),
      Endianness_Disable           = 0b01,
      Integer_Bytes_Swap_Indicator = (1 << 1),
      Endianness_Enable            = 0b11,

      Data_Copy = (1 << 2),
      SRX_ENUM_BITMASK
    };

    static_assert(sizeof(T) == sizeof(byte), "Invalid data unit size");

public:
    using ValueType     = T;
    using ConstIterator = const ValueType*;

public:
    ReadOnlyDataStream() = default;
    ReadOnlyDataStream(ConstIterator it,
                       size_t        size,
                       uint8         flags = EOption::Default);
    ReadOnlyDataStream(ConstIterator first,
                       ConstIterator last,
                       uint8         flags = EOption::Default);

    virtual ~ReadOnlyDataStream() override;

    ReadOnlyDataStream(const ReadOnlyDataStream& other);
    // cppcheck-suppress operatorEqToSelf
    ReadOnlyDataStream& operator=(const ReadOnlyDataStream& other);

    ReadOnlyDataStream(ReadOnlyDataStream&& other);
    ReadOnlyDataStream& operator=(ReadOnlyDataStream&& other) noexcept;

    inline ConstIterator begin() const { return _begin; }
    inline ConstIterator end() const { return _end; }

    inline const ValueType* GetData() const { return _begin; }

    /**
     * @brief Retrieve total size of items.
     *
     * @return total number of items.
     */
    inline size_t GetSize() const noexcept { return (_end - _begin); }

    /**
     * @brief Retrieve number of remainded items.
     *
     * @return remaided number of items.
     */
    inline size_t GetTail() const noexcept { return (_end - _current); }

    inline const ValueType& operator[](size_t i) const noexcept
    {
      return _begin[i];
    }

    /**
     * @brief Retrieve value of item by index from begin of data.
     *
     * @param i - index of the item
     * @param val - out value
     * @return True if item is into data and saved into val, else False.
     */
    bool Get(size_t i, ValueType& val) const;

    /**
     * @brief Do step forward.
     *
     * @note: There is no any checking of stream state and range of step.
     *
     * @param step - step size.
     */
    inline void Advance(size_t step = 1) noexcept { _current += step; }

    /**
     * @brief Do checked step forward if stream has more remained data than step
     * size.
     *
     * @param step - step size.
     * @return True if position of stream moved forward on step size, else
     * False.
     */
    bool AdvanceSafe(size_t step = 1);

    /**
     * @brief Retrieve current value of the stream and advance forward.
     *
     * @note: There is no checking stream state or range of data.
     * Use Read(&value) if there is no guarantee that stream or range is valid.
     *
     * @return value of the item in current position of the stream.
     */
    inline T Next();

    /**
     * @brief Read data into from stream to buffer.
     *
     * The same as `Read` function but without checking stream state or range of
     * data.
     *
     * @param buffer - pointer to a storage buffer
     * @param length - number of read items
     */
    inline void Pull(ValueType* buffer, size_t length);

    /**
     * @brief Read integer value from stream to the value.
     *
     * @note: There is no checking stream state or range of data.
     *  It swap bytes to the little endian order if the system has little
     * endian.
     *
     * @param value - storage for integer value
     */
    template<
      typename Int,
      typename Enable = RFY_TYPENAME TEnableIf_Type<TIsIntegral_Value<Int>>>
    void Pull(Int& value);

    // Interface Stream
    virtual bool Check(EAccessMode mode) override
    {
      return mode == EAccessMode::Read && IsOpen();
    }

    virtual bool IsOpen() const override { return _begin && _end; }
    virtual bool EndOfFile() override
    {
      return _current == nullptr || _current >= _end;
    }

    virtual StringView   GetName() override;
    virtual const Error& GetError() override { return *GetErrorObject(); }

    virtual int32 GetLength() override { return static_cast<int32>(GetSize()); }
    virtual int32 GetPosition() override
    {
      return static_cast<int32>(_current - _begin);
    }

    virtual bool Seek(int32 pos) override;
    virtual bool Peek(byte& value) override;

    virtual int32 Read(void* buffer, int32 length) override;
    virtual bool  Read(byte& value) override
    {
      return Read(&value, sizeof(byte)) == sizeof(byte);
    }

    virtual int32 Write(const void* buffer, int32 length) override;

    virtual bool Reset() override;

private:
    inline void   Release();
    inline Error* GetErrorObject();

private:
    ConstIterator _begin   = nullptr;
    ConstIterator _current = nullptr;
    ConstIterator _end     = nullptr;

    ValueType* _data    = nullptr;
    uint8      _options = EOption::Default_Options;

    TUniquePointer<Error> _error;
  };

  template<typename T>
  ReadOnlyDataStream<T>::ReadOnlyDataStream(
    ConstIterator it,
    size_t        size,
    uint8         options /* = Default_Options */)
    : _begin(it)
    , _current(it)
    , _end(it + size)
    , _data(options & EOption::Data_Copy ? new ValueType[size] : nullptr)
    , _options(options)
  {
    if (_data)
    {
      std::copy_n(it, size, _data);
      _begin = _current = _data;
      _end              = _data + size;
    }

    if ((_options & EOption::Integer_Bytes_Swap_Custom) == 0)
    {
      if (IsLittleEndian())
        _options |= EOption::Integer_Bytes_Swap_Indicator;
      else
        _options &= ~EOption::Integer_Bytes_Swap_Indicator;
    }
  }

  template<typename T>
  ReadOnlyDataStream<T>::ReadOnlyDataStream(
    ConstIterator first,
    ConstIterator last,
    uint8         options /* = Default_Options */)
    : ReadOnlyDataStream(first, last - first, options)
  {}

  template<typename T>
  ReadOnlyDataStream<T>::~ReadOnlyDataStream()
  {
    Release();
  }

  template<typename T>
  ReadOnlyDataStream<T>::ReadOnlyDataStream(const ReadOnlyDataStream& other)
  {
    if (!other._data)
    {
      _begin   = other._begin;
      _current = other._current;
      _end     = other._end;
      _data    = nullptr;
    }
    else
    {
      const size_t size = other.GetSize();
      _data             = new ValueType[size];

      std::copy(other._begin, other._end, _data);

      _begin   = _data;
      _current = _data + (other._current - other._begin);
      _end     = _data + size;
    }

    _options = other._options;
    if (other._error)
      *GetErrorObject() = *other._error;
    else if (_error)
      _error->Reset();
  }

  template<typename T>
  ReadOnlyDataStream<T>& ReadOnlyDataStream<T>::operator=(
    const ReadOnlyDataStream& other)
  {
    if (this == &other)
    {
      RFY_NOEXCEPT("operatorEqToSelf");
      return *this;
    }

    if (other._data == nullptr)
    {
      Release();

      _begin   = other._begin;
      _current = other._current;
      _end     = other._end;
    }
    else
    {
      const size_t size = other.GetSize();
      if (_data == nullptr || size > GetSize())
      {
        delete[] _data;
        _data = new ValueType[size];
      }

      std::copy(other._begin, other._end, _data);

      _begin   = _data;
      _current = _data + (other._current - other._begin);
      _end     = _data + size;
    }

    _options = other._options;
    if (other._error)
      *GetErrorObject() = *other._error;
    else if (_error)
      _error->Reset();

    return *this;
  }

  template<typename T>
  ReadOnlyDataStream<T>::ReadOnlyDataStream(ReadOnlyDataStream&& other)
    : _begin(other._begin)
    , _current(other._current)
    , _end(other._end)
    , _data(other._data)
    , _options(other._options)
    , _error(std::move(other._error))
  {
    other._data = nullptr;
  }

  template<typename T>
  ReadOnlyDataStream<T>& ReadOnlyDataStream<T>::operator=(
    ReadOnlyDataStream&& other) noexcept
  {
    if (this == &other)
    {
      RFY_NOEXCEPT("operatorEqToSelf");
      return *this;
    }

    Release();

    _begin   = other._begin;
    _current = other._current;
    _end     = other._end;
    _data    = other._data;
    _options = other._options;
    _error   = std::move(other._error);

    other._data = nullptr;
    return *this;
  }

  template<typename T>
  bool ReadOnlyDataStream<T>::Get(size_t i, ValueType& val) const
  {
    if (i >= GetSize())
      return false;

    val = _begin[i];
    return true;
  }

  template<typename T>
  bool ReadOnlyDataStream<T>::AdvanceSafe(size_t step /* = 1 */)
  {
    if (!IsOpen() || EndOfFile())
      return false;

    if (step >= GetTail())
      return false;

    _current += step;
    return true;
  }

  template<typename T>
  inline T ReadOnlyDataStream<T>::Next()
  {
    RFY_CHECK(IsOpen() && !EndOfFile());

    T tmp = _current[0];
    ++_current;
    return tmp;
  }

  template<typename T>
  inline void ReadOnlyDataStream<T>::Pull(ValueType* buffer, size_t length)
  {
    RFY_CHECK(IsOpen() && length < GetTail());

    std::copy_n(_current, length, buffer);
    _current += length;
  }

  template<typename T>
  template<typename Int, typename Enable>
  void ReadOnlyDataStream<T>::Pull(Int& value)
  {
    if constexpr (sizeof(Int) == 1)
    {
      value = _current[0];
      Advance();
      return;
    }

    Pull(reinterpret_cast<ValueType*>(&value), sizeof(value));

    if (_options & EOption::Integer_Bytes_Swap_Indicator)
      value = Utils::SwapBytes(value);
  }

  template<typename T>
  StringView ReadOnlyDataStream<T>::GetName()
  {
    return IsOpen() ? StringView(reinterpret_cast<const char*>(_begin),
                                 std::min(GetSize(), size_t(8)))
                    : StringView();
  }

  template<typename T>
  bool ReadOnlyDataStream<T>::Seek(int32 pos)
  {
    if (!IsOpen())
    {
      RFY_MAKE_ERR(GetErrorObject(),
                   Error::Invalid_State,
                   "Seek(): Stream isn't open");
      return false;
    }

    if (pos < 0 || static_cast<size_t>(pos) >= GetSize())
    {
      RFY_MAKE_ERR(GetErrorObject(), Error::Out_Of_Range, "Seek()");
      return false;
    }

    _current = _begin + pos;
    return true;
  }

  template<typename T>
  bool ReadOnlyDataStream<T>::Peek(byte& value)
  {
    if (EndOfFile())
      return false;

    value = _current[0];
    return true;
  }

  template<typename T>
  int32 ReadOnlyDataStream<T>::Read(void* buffer, int32 length)
  {
    if (buffer == nullptr || length < 0)
    {
      RFY_MAKE_ERR(GetErrorObject(),
                   Error::Invalid_Argument,
                   "Read(): length={}",
                   length);
      return -1;
    }

    if (!IsOpen() || EndOfFile())
    {
      RFY_MAKE_ERR(GetErrorObject(), Error::Invalid_State, "Read()");
      return -1;
    }

    int32 n = std::min(length, static_cast<int32>(GetTail()));
    std::copy_n(_current, n, reinterpret_cast<ValueType*>(buffer));
    _current += n;

    return n;
  }

  template<typename T>
  int32 ReadOnlyDataStream<T>::Write(const void* buffer, int32 length)
  {
    RFY_MAKE_ERR(GetErrorObject(),
                 Error::Not_Permitted,
                 "Try write to read only stream");
    return -1;
  }

  template<typename T>
  bool ReadOnlyDataStream<T>::Reset()
  {
    _current = _begin;
    _error.reset();

    return true;
  }

  template<typename T>
  inline void ReadOnlyDataStream<T>::Release()
  {
    if (_data)
    {
      delete[] _data;
      _data = nullptr;
    }
  }

  template<typename T>
  inline Error* ReadOnlyDataStream<T>::GetErrorObject()
  {
    if (!_error)
      _error = MakeUnique<Error>();

    return _error.get();
  }
}

namespace Ruffy
{
  using ReadOnlyMemoryStream = Utils::ReadOnlyDataStream<byte>;
}
