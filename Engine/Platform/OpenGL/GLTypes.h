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

#include <glad/glad.h>

// #define SOREX_OPENGL_TEXTURE_NUMBER (8)

#ifdef SOREX_DEBUG_HIGH
#  define SOREX_OPENGL_DEBUG_OUTPUT (1)
#endif

#if defined(SOREX_OPENGL_DEBUG_OUTPUT) && defined(GL_DEBUG_OUTPUT)
#  define SRX_OPENGL_FN(expr) expr

#elif defined(SOREX_DEBUG_MEDIUM)
#  define SRX_OPENGL_FN(expr)                                           \
    ((void)(expr));                                                     \
    SRX_ASSERT_MSG(                                                     \
      !Sorex::Graphics::OpenGL::CheckErrors(#expr, __FILE__, __LINE__), \
      #expr)

#else
#  define SRX_OPENGL_FN(expr) ((void)(expr))

#endif


namespace Sorex::Graphics
{
  using GLString     = Sorex::BasicString<GLchar>;
  using GLStringView = Sorex::BasicStringView<GLchar>;

  namespace OpenGL
  {
    namespace Concept
    {
      template<typename T>
      concept GLBuiltin =
        std::is_same_v<T, GLbyte> || std::is_same_v<T, GLubyte>
        || std::is_same_v<T, GLushort> || std::is_same_v<T, GLshort>
        || std::is_same_v<T, GLuint> || std::is_same_v<T, GLint>
        || std::is_same_v<T, GLfloat> || std::is_same_v<T, GLdouble>;
    }

    bool GLAPIENTRY CheckErrors(const char* func, const char* file, int line);
  }
}
