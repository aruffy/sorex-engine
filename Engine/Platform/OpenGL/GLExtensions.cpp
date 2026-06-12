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

#include "GLExtensions.h"

namespace Sorex::Graphics
{
  GLExtensions::GLExtensions() SRX_NOEXCEPT
  {
    GLint n;
    SRX_OPENGL_CALL(glGetIntegerv(GL_NUM_EXTENSIONS, &n));
    mExtensions.reserve(n);
    for (GLint i = 0; i < n; i++)
    {
      if (const GLubyte* ext = glGetStringi(GL_EXTENSIONS, i))
      {
        StringView   extName = StringView(reinterpret_cast<const char*>(ext));
        const hash_t hash    = Sorex::Utils::GetHash(extName);
#ifdef SOREX_DEBUG_MEDIUM
        mExtensions.emplace(hash, String(extName));
#else
        mExtensions.insert(hash);
#endif
      }
    }

    SRX_DEBUG("[GLExtensions] Extensions loaded: {}", mExtensions.size());
  }
}
