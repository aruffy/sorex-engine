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

#include "DesktopGraphicsFramework.h"

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Sorex::Platform
{
  DesktopGraphicsFramework& DesktopGraphicsFramework::GetInstance() SRX_NOEXCEPT
  {
    static DesktopGraphicsFramework _instance;
    return _instance;
  }

  Status DesktopGraphicsFramework::Initialize()
  {
    SRX_CLSFUN_TRACE();
    SRX_CHECK(!mIsInitialized);

    if (mIsInitialized)
      return SRX_OK;

    if (glfwInit() == GLFW_FALSE)
    {
      const char* desc;
      const int   code = glfwGetError(&desc);
      // FIXME: Make GLFW error_code
      return SRX_STATUS_MSG(EStatusCode::Not_Permitted,
                            "[GLFW] Initialization failed ({}): {}",
                            code,
                            desc);
    }

    /* glfwSetErrorCallback([](int errorId, const char errorMsg) SRX_NOEXCEPT {
      SRX_ERROR("[DesktopGraphicsFramework] GLFW error ({}): {}",
                errorId,
                errorMsg);
    }); */

    mIsInitialized = true;
    return SRX_OK;
  }
}  // namespace
