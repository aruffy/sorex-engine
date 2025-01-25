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
#include <Sorex/Math/SxMatrix3x3.h>
#include <Sorex/Memory/SxMemoryBlock.h>

#include "GLTypes.h"

namespace Sorex::Graphics
{
  struct GLUniformInfo
  {
    hash_t hash     = 0;
    GLint  location = -1;
    GLint  count    = 0;
    GLenum type     = GL_NONE;

#ifdef SOREX_DEBUG_MEDIUM
    GLString name;
#endif

    static bool IsValid(const GLUniformInfo& uniform) SRX_NOEXCEPT;
  };

  class GLRenderDevice;
  class GLUniform final
  {
public:
    explicit GLUniform(const GLUniformInfo& uniform) SRX_NOEXCEPT;
    ~GLUniform();

    GLUniform(const GLUniform& other)            = delete;
    GLUniform& operator=(const GLUniform& other) = delete;

    GLUniform(GLUniform&& other) SRX_NOEXCEPT;
    GLUniform& operator=(GLUniform&& other) SRX_NOEXCEPT;


    EStatusCode Apply() SRX_NOEXCEPT;

    inline hash_t GetHash() const { return mParams.hash; }
    inline GLenum GetType() const { return mParams.type; }
    inline GLint  GetCount() const { return mParams.count; }
    inline GLint  GetLocation() const { return mParams.location; }

#ifdef SOREX_DEBUG_MEDIUM
    const GLString& GetName() const { return mParams.name; }
#endif

    template<OpenGL::Concept::GLBuiltin T>
    EStatusCode SetValue(T value) SRX_NOEXCEPT
    {
      return SetValueInternal(&value, sizeof(T));
    }

    template<OpenGL::Concept::GLBuiltin T>
    EStatusCode GetValue(T& value) const SRX_NOEXCEPT
    {
      return GetValueInternal(&value, sizeof(T));
    }

    template<OpenGL::Concept::GLBuiltin T>
    EStatusCode SetValues(const TVector<T>& values) SRX_NOEXCEPT
    {
      SRX_CHECK(!values.empty());
      return SetValueInternal(values.data(), values.size() * sizeof(T));
    }

    template<OpenGL::Concept::GLBuiltin T, size_t N>
    EStatusCode SetVector(const TArray<T, N>& vec) SRX_NOEXCEPT
    {
      static_assert(N > 0, "[GLUniform] Invalid vector size");
      return SetValueInternal(vec.data(), N * sizeof(T));
    }

    template<OpenGL::Concept::GLBuiltin T, size_t N>
    EStatusCode SetVectors(const TVector<TArray<T, N>>& vecs) SRX_NOEXCEPT
    {
      static_assert(N > 0, "[GLUniform] Invalid vector size");
      SRX_CHECK(!vecs.empty());
      return SetValueInternal(&vecs[0][0], N * sizeof(T) * vecs.size());
    }

    template<OpenGL::Concept::GLFloatingPoint T>
    EStatusCode SetMatrix(const T*     mat,
                          const size_t m,
                          const size_t n) SRX_NOEXCEPT
    {
      return SetValueInternal(static_cast<const void*>(mat), m * n * sizeof(T));
    }

    template<OpenGL::Concept::GLFloatingPoint T>
    EStatusCode SetMatrix(const Math::TMatrix3x3<T>& matrix) SRX_NOEXCEPT
    {
      return SetValueInternal(static_cast<const void*>(matrix.GetValuePtr()),
                              3 * 3 * sizeof(T));
    }

private:
    void        Allocate() SRX_NOEXCEPT;
    EStatusCode SetValueInternal(const void* data, size_t size) SRX_NOEXCEPT;
    EStatusCode GetValueInternal(void* data, size_t size) const SRX_NOEXCEPT;

private:
    enum class ESetterType : uint8
    {
      None,
      Int,
      UInt,
      Float,
      Matrix
    };

    union Setter
    {
      void (*setUniformiv)(GLint location, GLsizei count, const GLint* value);
      void (*setUniformuiv)(GLint location, GLsizei count, const GLuint* value);
      void (*setUniformfv)(GLint location, GLsizei count, const GLfloat* value);
      void (*setUniformMatrixfv)(GLint          location,
                                 GLsizei        count,
                                 GLboolean      transpose,
                                 const GLfloat* value);
    };

private:
    GLUniformInfo mParams;
    MemoryBlock   mValue;

    Setter      mSetter;
    ESetterType mSetterType;

    bool mIsUpdated;
  };
}  // namespace
