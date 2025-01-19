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

#include "GLUniform.h"

#include "GLRenderDevice.h"

namespace
{
  /* const Sorex::THash<Sorex::Graphics::GLStringView> glHasher =
    Sorex::THash<Sorex::Graphics::GLStringView>(); */
}

namespace Sorex::Graphics
{
  bool GLUniformInfo::IsValid(const GLUniformInfo& uniform) SRX_NOEXCEPT
  {
    return (uniform.location >= 0 && uniform.count > 0
            && uniform.type != GL_NONE && uniform.hash != 0);
  }

  GLUniform::GLUniform(const GLUniformInfo& uniform) SRX_NOEXCEPT
    : mParams(uniform)
    , mIsUpdated(false)  // https://docs.gl/es3/glUniform
  {
    SRX_CHECK(GLUniformInfo::IsValid(uniform));
    Allocate();
  }

  GLUniform::~GLUniform()
  {
    if (mValue.IsValid())
      delete[] mValue.GetData();
  }

  void GLUniform::Allocate() SRX_NOEXCEPT
  {
    SRX_CHECK(mValue.IsValid() == false);

    int32 size = 0;
    switch (mParams.type)
    {
    case GL_BOOL:
    case GL_BYTE:
    case GL_SHORT:
    case GL_INT:
    case GL_SAMPLER_2D:
      size                 = sizeof(GLint);
      mSetterType          = ESetterType::Int;
      mSetter.setUniformiv = glUniform1iv;
      break;

    case GL_UNSIGNED_BYTE:
    case GL_UNSIGNED_SHORT:
    case GL_UNSIGNED_INT:
      size                  = sizeof(GLuint);
      mSetterType           = ESetterType::UInt;
      mSetter.setUniformuiv = glUniform1uiv;
      break;

    case GL_INT_VEC2:
    case GL_BOOL_VEC2:
      size                 = 2 * sizeof(GLint);
      mSetterType          = ESetterType::Int;
      mSetter.setUniformiv = glUniform2iv;
      break;

    case GL_UNSIGNED_INT_VEC2:
      size                  = 2 * sizeof(GLuint);
      mSetterType           = ESetterType::UInt;
      mSetter.setUniformuiv = glUniform2uiv;
      break;

    case GL_INT_VEC3:
    case GL_BOOL_VEC3:
      size                 = 3 * sizeof(GLint);
      mSetterType          = ESetterType::Int;
      mSetter.setUniformiv = glUniform3iv;
      break;

    case GL_UNSIGNED_INT_VEC3:
      size                  = 3 * sizeof(GLuint);
      mSetterType           = ESetterType::UInt;
      mSetter.setUniformuiv = glUniform3uiv;
      break;

    case GL_INT_VEC4:
    case GL_BOOL_VEC4:
      size                 = 4 * sizeof(GLint);
      mSetterType          = ESetterType::Int;
      mSetter.setUniformiv = glUniform4iv;
      break;

    case GL_UNSIGNED_INT_VEC4:
      size                  = 4 * sizeof(GLuint);
      mSetterType           = ESetterType::UInt;
      mSetter.setUniformuiv = glUniform4uiv;
      break;

    case GL_FLOAT:
      size                 = sizeof(GLfloat);
      mSetterType          = ESetterType::Float;
      mSetter.setUniformfv = glUniform1fv;
      break;

    case GL_FLOAT_VEC2:
      size                 = 2 * sizeof(GLfloat);
      mSetterType          = ESetterType::Float;
      mSetter.setUniformfv = glUniform2fv;
      break;

    case GL_FLOAT_VEC3:
      size                 = 3 * sizeof(GLfloat);
      mSetterType          = ESetterType::Float;
      mSetter.setUniformfv = glUniform3fv;
      break;

    case GL_FLOAT_VEC4:
      size                 = 4 * sizeof(GLfloat);
      mSetterType          = ESetterType::Float;
      mSetter.setUniformfv = glUniform4fv;
      break;

    case GL_FLOAT_MAT2:
      size                       = 4 * sizeof(GLfloat);
      mSetterType                = ESetterType::Matrix;
      mSetter.setUniformMatrixfv = glUniformMatrix2fv;
      break;

    case GL_FLOAT_MAT3:
      size                       = 9 * sizeof(GLfloat);
      mSetterType                = ESetterType::Matrix;
      mSetter.setUniformMatrixfv = glUniformMatrix3fv;
      break;

    case GL_FLOAT_MAT4:
      size                       = 16 * sizeof(GLfloat);
      mSetterType                = ESetterType::Matrix;
      mSetter.setUniformMatrixfv = glUniformMatrix4fv;
      break;

    default:
      SRX_NOENTRY("[GLUniform] Invalid uniform type");
      break;
    }

    if (const int32 alloc = std::max<int32>(mParams.count * size, 0); alloc > 0)
    {
      mValue = MemoryBlock(new byte[size], size);
      mValue.Fill(byte(0));
    }
  }

  EStatusCode GLUniform::Apply() SRX_NOEXCEPT
  {
    if (mIsUpdated == false)
      return EStatusCode::Ok;

    if (!mValue.IsValid())
      return EStatusCode::Invalid_State;

    if (mSetterType == ESetterType::Float) [[likely]]
      (*mSetter.setUniformfv)(mParams.location,
                              mParams.count,
                              static_cast<GLfloat*>(mValue.GetPtr()));

    else if (mSetterType == ESetterType::Int)
      (*mSetter.setUniformiv)(mParams.location,
                              mParams.count,
                              static_cast<GLint*>(mValue.GetPtr()));

    else if (mSetterType == ESetterType::Matrix)
      (*mSetter.setUniformMatrixfv)(mParams.location,
                                    mParams.count,
                                    GL_FALSE,
                                    static_cast<GLfloat*>(mValue.GetPtr()));
    else if (mSetterType == ESetterType::UInt) [[unlikely]]
      (*mSetter.setUniformuiv)(mParams.location,
                               mParams.count,
                               static_cast<GLuint*>(mValue.GetPtr()));
    else
      return EStatusCode::Invalid_State;

    mIsUpdated = false;
    return EStatusCode::Ok;
  }

  EStatusCode GLUniform::SetValueInternal(const void* data,
                                          size_t      size) SRX_NOEXCEPT
  {
    if (mValue.IsValid() == false)
    {
      SRX_NOENTRY("invalid GL uniform state");
      return EStatusCode::Invalid_State;
    }

    const EStatusCode bSuccess = mValue.Write(data, size);
    mIsUpdated                 = bSuccess == EStatusCode::Ok;
    return bSuccess;
  }

  EStatusCode GLUniform::GetValueInternal(void*  data,
                                          size_t size) const SRX_NOEXCEPT
  {
    if (mValue.IsValid() == false)
    {
      SRX_NOENTRY("invalid GL uniform state");
      return EStatusCode::Invalid_State;
    }

    return mValue.Read(data, size) != size ? EStatusCode::Out_Of_Range
                                           : EStatusCode::Ok;
  }
}
