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
#include <Sorex/Graphics/SxVertex.h>

#include "GLBuffer.h"
#include "GLVertexArray.h"


namespace Sorex::Graphics
{
  template<typename vertex_t>
  class GLQuadBatch
  {
    using VertexArray  = GLVertexArray<vertex_t>;
    using VertexBuffer = typename VertexArray::VertexBuffer;
    using IndexBuffer  = typename VertexArray::IndexBuffer;
    using IndexType    = typename IndexBuffer::IndexType;

public:
    static constexpr size_t kVertexNumberByQuad = 4;
    static constexpr size_t kIndexNumberByQuad  = 6;

    using VertexType = vertex_t;
    struct Quad
    {
      VertexType* tl;
      VertexType* tr;
      VertexType* bl;
      VertexType* br;
    };

public:
    GLQuadBatch(GLRenderDevice& glDevice, size_t capacity) SRX_NOEXCEPT;
    virtual ~GLQuadBatch() {}

    GLQuadBatch(const GLQuadBatch& other)            = delete;
    GLQuadBatch& operator=(const GLQuadBatch& other) = delete;

    SRX_INLINE void Clear() SRX_NOEXCEPT { mVtxArray.Clear(); }
    SRX_INLINE bool IsEmpty() const SRX_NOEXCEPT { return mVtxArray.IsEmpty(); }

    size_t GetSize() const SRX_NOEXCEPT;
    size_t GetCapacity() const { return mCapacity; }

    SRX_INLINE GLRenderDevice* GetRenderDevice() const
    {
      return mVtxArray.GetRenderDevice();
    }

    /**
     * @brief Allocate a quad.
     *  If the quad batch is full, the function will return Errror::No_Space.
     *  If the function return Error::Invalid_State, the quad batch is not
     * initialized. If the function return Error::Ok, the quad is allocated and
     * the quad parameter is filled.
     *
     * @param quad - out quad to fill;
     * @return error code.
     */
    Status Allocate(Quad& quad) SRX_NOEXCEPT;

protected:
    /**
     * @brief Clamp quad number according to a index buffer capacity and its
     * index type.
     *
     * @param quadNumber - desired quad number
     * @param minQuadNumber - minimal quad number
     * @return number of quad within possible range
     */
    template<typename IndexType>
    static size_t ClampQuadNumber(const size_t quadNumber,
                                  const size_t minQuadNumber = 1);

    const VertexArray& GetVertexArray() const { return mVtxArray; }

private:
    VertexArray  mVtxArray;
    const size_t mCapacity;
  };

  template<typename VertexType>
  template<typename IndexType>
  size_t GLQuadBatch<VertexType>::ClampQuadNumber(const size_t quadNumber,
                                                  const size_t minQuadNumber)
  {
    constexpr size_t kMaxIndexCapacity =
      static_cast<size_t>(std::numeric_limits<IndexType>::max());
    constexpr size_t kMaxQuadNumber = kMaxIndexCapacity / kVertexNumberByQuad;

    if (minQuadNumber > kMaxQuadNumber)
    {
      SRX_NOENTRY("invaid minimal quad number");
      return kMaxQuadNumber;
    }

    return std::max<size_t>(std::min<size_t>(quadNumber, kMaxQuadNumber),
                            minQuadNumber);
  }

  template<typename VertexType>
  GLQuadBatch<VertexType>::GLQuadBatch(GLRenderDevice& glDevice,
                                       size_t          capacity) SRX_NOEXCEPT
    : mCapacity(ClampQuadNumber<IndexType>(capacity, 8))
    , mVtxArray(&glDevice)
  {
    SRX_CHECK(capacity == mCapacity);
    SRX_VERIFY(mVtxArray
                 .Initialize(mCapacity * kVertexNumberByQuad,
                             mCapacity * kIndexNumberByQuad)
                 .Ok());
  }

  template<typename VertexType>
  size_t GLQuadBatch<VertexType>::GetSize() const SRX_NOEXCEPT
  {
    return mVtxArray.GetVertexNum() / kVertexNumberByQuad;
  }

  template<typename VertexType>
  Status GLQuadBatch<VertexType>::Allocate(Quad& quad) SRX_NOEXCEPT
  {
    if (const size_t quadNumber = GetSize(); quadNumber > mCapacity)
      return SRX_STATUS(EStatusCode::No_Space);

    const IndexType vtxBaseIndex =
      static_cast<IndexType>(mVtxArray.GetVertexNum());

    VertexType* vertices = mVtxArray.AllocateVertex(kVertexNumberByQuad);
    IndexType*  indices  = mVtxArray.AllocateIndecies(kIndexNumberByQuad);

    // @NOTE: should never happen, we've checked the capacity
    SRX_ASSERT(vertices && indices);

    quad.tl = vertices;
    quad.bl = vertices + 1;
    quad.br = vertices + 2;
    quad.tr = vertices + 3;

    indices[0] = vtxBaseIndex + 0;
    indices[1] = vtxBaseIndex + 1;
    indices[2] = vtxBaseIndex + 2;
    indices[3] = vtxBaseIndex + 0;
    indices[4] = vtxBaseIndex + 2;
    indices[5] = vtxBaseIndex + 3;

    return SRX_OK;
  }

  // TODO: Allocate(TSpan<Quad>);

  class GLTexBatch final: public GLQuadBatch<Vertex::V2F_C4B_TC2F>
  {
    using VertexType = Vertex::V2F_C4B_TC2F;
    using Quad       = typename GLQuadBatch<VertexType>::Quad;

public:
    GLTexBatch(GLRenderDevice& glDevice, size_t capacity) SRX_NOEXCEPT;

    Status Flush();
    void   Draw(const TArray<Point, 4>& texcoord,
                const TArray<Point, 4>& screenPoints,
                Color                   color);
  };
}  // namespace
