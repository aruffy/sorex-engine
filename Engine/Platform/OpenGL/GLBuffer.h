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

#include <Sorex/Graphics/SxVertex.h>

#include "GLResource.h"

namespace Sorex::Graphics
{
  class GLRenderDevice;
  namespace OpenGL
  {
    class Buffer
    {
  public:
      Buffer(GLRenderDevice* glRenderDevice, GLResourceType type);
      virtual ~Buffer() {}

      Buffer(const Buffer& other)            = delete;
      Buffer& operator=(const Buffer& other) = delete;

      SRX_INLINE GLResourceReference* GetResourceToken()
      {
        return _glToken.get();
      }
      SRX_INLINE const GLResourceReference* GetResourceToken() const
      {
        return _glToken.get();
      }

      SRX_INLINE GLRenderDevice* GetRenderDevice()
      {
        return _glToken ? _glToken->GetRenderDevice() : nullptr;
      }

      SRX_INLINE GLResourceType GetType() const { return _type; }

  private:
      GLResourceToken _glToken;
      GLResourceType  _type;
    };
  }  // namespace OpenGL

  struct GLBufferData
  {
    size_t      size     = 0;
    size_t      capacity = 0;
    const byte* memptr   = nullptr;

    bool IsEmpty() const { return (memptr == nullptr || size == 0); }
  };

  class GLRenderDevice;
  template<typename ValueType>
  class GLBuffer: public OpenGL::Buffer
  {
public:
    using Container     = TVector<ValueType>;
    using Iterator      = typename Container::iterator;
    using ConstIterator = typename Container::const_iterator;

public:
    GLBuffer(GLRenderDevice* glRenderDevice,
             GLResourceType  type,
             size_t          capacity);
    virtual ~GLBuffer() override {}

    GLBuffer(const GLBuffer& other)            = delete;
    GLBuffer& operator=(const GLBuffer& other) = delete;

    SRX_INLINE Iterator begin() { return _buffer.begin(); }
    SRX_INLINE Iterator end() { return _buffer.end(); }

    SRX_INLINE ConstIterator cbegin() { return _buffer.cbegin(); }
    SRX_INLINE ConstIterator cend() { return _buffer.cend(); }

    SRX_INLINE bool IsEmpty() const { return _buffer.empty(); }
    SRX_INLINE void Clear() { _buffer.clear(); }

    size_t GetSize() const { return _buffer.size(); }
    size_t GetCapacity() const { return _buffer.capacity(); }

    GLBufferData GetData() const;

    RFY_NODISCARD ValueType* Allocate(size_t number) RFY_NOEXCEPT;
    bool                     PushBack(const ValueType& value) RFY_NOEXCEPT;

private:
    Container _buffer;
  };

  template<typename VertexType>
  class GLVertexBuffer final: public GLBuffer<VertexType>
  {
public:
    GLVertexBuffer(GLRenderDevice* glRenderDevice, size_t capacity)
      : GLBuffer<VertexType>(glRenderDevice,
                             GLResourceType::VertexBuffer,
                             capacity)
    {}

    static SRX_INLINE const VertexLayout& GetVertexLayout()
    {
      return VertexType::GetLayout();
    }
  };

  template<typename index_t = uint16>
  class GLIndexBuffer final: public GLBuffer<index_t>
  {
    template<typename T>
    static constexpr bool TIsIndexType = TIsSame_Value<index_t, T>;

    static constexpr bool kIsValidIndexType =
      (TIsIndexType<byte> || TIsIndexType<uint16> || TIsIndexType<uint32>);

    static_assert(kIsValidIndexType, "invalid index type");

public:
    using IndexType = index_t;

public:
    GLIndexBuffer(GLRenderDevice* glRenderDevice, size_t capacity)
      : GLBuffer<IndexType>(glRenderDevice,
                            GLResourceType::IndexBuffer,
                            capacity)
    {}
  };

  template<typename ValueType>
  GLBuffer<ValueType>::GLBuffer(GLRenderDevice* glRenderDevice,
                                GLResourceType  type,
                                size_t          capacity)
    : OpenGL::Buffer(glRenderDevice, type)
  {
    _buffer.reserve(capacity);
  }

  template<typename ValueType>
  GLBufferData GLBuffer<ValueType>::GetData() const
  {
    return GLBufferData{ _buffer.size() * sizeof(ValueType),
                         _buffer.capacity() * sizeof(ValueType),
                         reinterpret_cast<const byte*>(_buffer.data()) };
  }

  template<typename ValueType>
  RFY_NODISCARD ValueType* GLBuffer<ValueType>::Allocate(size_t number)
    RFY_NOEXCEPT
  {
    const size_t size = _buffer.size();
    if (size + number > _buffer.capacity())
      return nullptr;

    _buffer.resize(size + number);
    return &_buffer[size];
  }

  template<typename ValueType>
  bool GLBuffer<ValueType>::PushBack(const ValueType& value) RFY_NOEXCEPT
  {
    if (_buffer.size() >= _buffer.capacity())
      return false;

    _buffer.push_back(value);
    return true;
  }
}  // namespace
