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

#include "GLTypes.h"
#include "GLResourceToken.h"

namespace Sorex::Graphics
{
  class GLRenderDevice;
  namespace OpenGL
  {
    class Buffer
    {
  public:
      Buffer(GLRenderDevice* glRenderDevice, GLResourceType type) SRX_NOEXCEPT;
      virtual ~Buffer() = default;

      Buffer(const Buffer& other)            = delete;
      Buffer& operator=(const Buffer& other) = delete;

      SRX_INLINE GLResourceReference* GetResourceToken()
      {
        return mGlToken.get();
      }

      SRX_INLINE const GLResourceReference* GetResourceToken() const
      {
        return mGlToken.get();
      }

      SRX_INLINE GLRenderDevice* GetRenderDevice()
      {
        return mGlToken ? mGlToken->GetRenderDevice() : nullptr;
      }

      SRX_INLINE GLResourceType GetType() const { return mType; }

  private:
      GLResourceToken mGlToken;
      GLResourceType  mType;
    };
  }  // namespace OpenGL

  struct GLBufferData
  {
    GLsizei       size     = 0;
    GLsizei       capacity = 0;
    const GLbyte* memptr   = nullptr;

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
             size_t          capacity) SRX_NOEXCEPT;
    virtual ~GLBuffer() override {}

    GLBuffer(const GLBuffer& other)            = delete;
    GLBuffer& operator=(const GLBuffer& other) = delete;

    SRX_INLINE Iterator begin() { return mBuffer.begin(); }
    SRX_INLINE Iterator end() { return mBuffer.end(); }

    SRX_INLINE ConstIterator cbegin() { return mBuffer.cbegin(); }
    SRX_INLINE ConstIterator cend() { return mBuffer.cend(); }

    SRX_INLINE bool IsEmpty() const { return mBuffer.empty(); }
    SRX_INLINE void Clear() { mBuffer.clear(); }

    size_t GetSize() const { return mBuffer.size(); }
    size_t GetCapacity() const { return mBuffer.capacity(); }

    GLBufferData GetData() const;

    SRX_NODISCARD ValueType* Allocate(size_t number) SRX_NOEXCEPT;
    bool                     PushBack(const ValueType& value) SRX_NOEXCEPT;

private:
    Container mBuffer;
  };

  template<typename VertexType>
  class GLVertexBuffer final: public GLBuffer<VertexType>
  {
public:
    GLVertexBuffer(GLRenderDevice* glRenderDevice, size_t capacity) SRX_NOEXCEPT
      : GLBuffer<VertexType>(glRenderDevice,
                             GLResourceType::VertexBuffer,
                             capacity)
    {}

    static SRX_INLINE const VertexLayout& GetVertexLayout()
    {
      return VertexType::GetLayout();
    }
  };

  template<typename index_t = GLshort>
  class GLIndexBuffer final: public GLBuffer<index_t>
  {
    template<typename T>
    static constexpr bool TIsIndexType = std::is_same_v<index_t, T>;

    static constexpr bool kIsValidIndexType =
      (TIsIndexType<GLbyte> || TIsIndexType<GLshort> || TIsIndexType<GLint>);

    static_assert(kIsValidIndexType, "invalid index type");

public:
    using IndexType = index_t;

public:
    GLIndexBuffer(GLRenderDevice* glRenderDevice, size_t capacity) SRX_NOEXCEPT
      : GLBuffer<IndexType>(glRenderDevice,
                            GLResourceType::IndexBuffer,
                            capacity)
    {}
  };

  template<typename ValueType>
  GLBuffer<ValueType>::GLBuffer(GLRenderDevice* glRenderDevice,
                                GLResourceType  type,
                                size_t          capacity) SRX_NOEXCEPT
    : OpenGL::Buffer(glRenderDevice, type)
  {
    mBuffer.reserve(capacity);
  }

  template<typename ValueType>
  GLBufferData GLBuffer<ValueType>::GetData() const
  {
    return GLBufferData{ mBuffer.size() * sizeof(ValueType),
                         mBuffer.capacity() * sizeof(ValueType),
                         reinterpret_cast<const GLbyte*>(mBuffer.data()) };
  }

  template<typename ValueType>
  SRX_NODISCARD ValueType* GLBuffer<ValueType>::Allocate(size_t number)
    SRX_NOEXCEPT
  {
    const size_t size = mBuffer.size();
    if (size + number > mBuffer.capacity())
      return nullptr;

    mBuffer.resize(size + number);
    return &mBuffer[size];
  }

  template<typename ValueType>
  bool GLBuffer<ValueType>::PushBack(const ValueType& value) SRX_NOEXCEPT
  {
    if (mBuffer.size() == mBuffer.capacity())
      return false;

    mBuffer.push_back(value);
    return true;
  }
}  // namespace
