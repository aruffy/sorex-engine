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

#include "GLBuffer.h"

namespace Sorex::Graphics
{
  template<typename VertexType, typename IndexType = uint16>
  class GLVertexArray
  {
public:
    using VertexBuffer = GLVertexBuffer<VertexType>;
    using IndexBuffer  = GLIndexBuffer<IndexType>;

public:
    explicit GLVertexArray(GLRenderDevice* glRenderDevice);

    Status Initialize(size_t vtxCapacity, size_t indxCapacity = 0) SRX_NOEXCEPT;

    VertexBuffer*       GetVertexBuffer() { return mVtxBuffer.get(); }
    const VertexBuffer* GetVertexBuffer() const { return mVtxBuffer.get(); }

    IndexBuffer*       GetIndexBuffer() { return mIndxBuffer.get(); }
    const IndexBuffer* GetIndexBuffer() const { return mIndxBuffer.get(); }

    GLResourceReference*       GetResourceToken() { return mGlToken.get(); }
    const GLResourceReference* GetResourceToken() const
    {
      return mGlToken.get();
    }

    SRX_INLINE VertexType* AllocateVertex(const size_t num) SRX_NOEXCEPT
    {
      return mVtxBuffer ? mVtxBuffer->Allocate(num) : nullptr;
    }

    SRX_INLINE size_t GetVertexCapacity() const SRX_NOEXCEPT
    {
      return mVtxBuffer ? mVtxBuffer->GetCapacity() : 0;
    }

    SRX_INLINE size_t GetVertexNum() const SRX_NOEXCEPT
    {
      return mVtxBuffer ? mVtxBuffer->GetSize() : 0;
    }

    SRX_INLINE IndexType* AllocateIndecies(const size_t num) SRX_NOEXCEPT
    {
      return mIndxBuffer ? mIndxBuffer->AllocateIndecies(num) : nullptr;
    }

    SRX_INLINE size_t GetIndexNum() const SRX_NOEXCEPT
    {
      return mIndxBuffer ? mIndxBuffer->GetSize() : 0;
    }

    SRX_INLINE size_t GetIndexCapacity() const SRX_NOEXCEPT
    {
      return mIndxBuffer ? mIndxBuffer->GetCapacity() : 0;
    }

    bool IsEmpty() const SRX_NOEXCEPT
    {
      if (mVtxBuffer && mVtxBuffer->IsEmpty())
      {
        SRX_CHECK(!mIndxBuffer || mIndxBuffer->IsEmpty());
        return true;
      }

      return false;
    }

    void Clear() SRX_NOEXCEPT
    {
      if (mVtxBuffer)
        mVtxBuffer->Clear();
      if (mIndxBuffer)
        mIndxBuffer->Clear();
    }

private:
    GLRenderDevice* GetRenderDevice()
    {
      return mGlToken ? mGlToken->GetRenderDevice() : nullptr;
    }

    template<typename T>
    T* CreateBuffer(TUniquePointer<T>& buffer,
                    const size_t       capacity) SRX_NOEXCEPT;

private:
    GLResourceToken mGlToken;

    TUniquePointer<VertexBuffer> mVtxBuffer;
    TUniquePointer<IndexBuffer>  mIndxBuffer;
  };

  template<typename VertexType, typename IndexType>
  GLVertexArray<VertexType, IndexType>::GLVertexArray(
    GLRenderDevice* glRenderDevice)
    : mGlToken(AllocateResource(glRenderDevice, GLResourceType::VertexArray))
  {}


  template<typename VertexType, typename IndexType>
  Status GLVertexArray<VertexType, IndexType>::Initialize(
    size_t vtxCapacity,
    size_t indxCapacity /* = 0 */) SRX_NOEXCEPT
  {
    SRX_CHECK_MSG(vtxCapacity > 0, "invalid vertex capacity");

    GLRenderDevice* glDevice = GetRenderDevice();
    if (glDevice == nullptr)
      return SRX_STATUS_MSG(EStatusCode::Invalid_State, "invalid device token");

    mVtxBuffer = MakeUnique<VertexBuffer>(glDevice, vtxCapacity);

    if (indxCapacity > 0)
      mIndxBuffer = MakeUnique<IndexBuffer>(glDevice, indxCapacity);
    else
      mIndxBuffer = nullptr;

    return SRX_OK;
  }

  template<typename Vertex, typename Index>
  Status FlushVertexArray(GLVertexArray<Vertex, Index>& vtxArray);
}  // namespace Ruffy::Graphics
