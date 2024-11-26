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

namespace
{
  // TODO: Add error handling
  void OnFrameworkError(int errorId, const char* errorMsg) SRX_NOEXCEPT
  {
    SRX_ERROR("[DesktopGraphicsFramework] GLFW error ({}): {}",
              errorId,
              errorMsg);
  }
}

namespace Sorex::Platform
{
  DesktopGraphicsFramework::~DesktopGraphicsFramework()
  {
    // Shutdown();
  }

  void DesktopGraphicsFramework::Attach(Director& director)
  {
    Director::Component::Attach(director);

    SRX_CHECK(!mDirector);
    director.AddListener(this);
    mDirector = &director;
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

    glfwSetErrorCallback(OnFrameworkError);
    mIsInitialized = true;

    // FIXME: move to director
    CreateWindow(L"HELLO", SizeInt{ 800, 640 });

    return SRX_OK;
  }

  void DesktopGraphicsFramework::Shutdown()
  {
    SRX_CLSFUN_TRACE();

    if (mDirector)
      mDirector->RemoveListener(this);

    if (!mIsInitialized)
      return;

    // FIXME: for_each(windows.begin(), windows.end(), []);
    if (mMainWindow)
      glfwDestroyWindow(mMainWindow);

    glfwTerminate();
    mIsInitialized = false;
  }

  TPair<Status, GLFWwindow*> DesktopGraphicsFramework::CreateWindow(
    const WString& title,
    const SizeInt& size) SRX_NOEXCEPT
  {
    SRX_CHECK(mIsInitialized);

    glfwWindowHint(GLFW_RESIZABLE,
                   true ? GL_TRUE : GL_FALSE);  // TODO: Add option

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // FIXME: Add macro to enable
    // glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);

    /* glfwWindowHint(GLFW_RED_BITS, _glCtxAttribs.redBits);
    glfwWindowHint(GLFW_GREEN_BITS, _glCtxAttribs.greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, _glCtxAttribs.blueBits);
    glfwWindowHint(GLFW_ALPHA_BITS, _glCtxAttribs.alphaBits);

    glfwWindowHint(GLFW_DEPTH_BITS, _glCtxAttribs.depthBits);
    glfwWindowHint(GLFW_STENCIL_BITS, _glCtxAttribs.stencilBits); */

    /*
#if (PLATFORM_MAC)
glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#endif
*/
    // TODO : monitor

    const String titleUtf8 =
      !title.empty() ? /* StrUtils::ToUtf8String(title) */ "TODO: Make name"
                     : "Main Window";
    GLFWwindow* window =
      glfwCreateWindow(size.width, size.height, titleUtf8.c_str(), NULL, NULL);

    if (window == nullptr)
    {
      const char* desc;
      int         code = glfwGetError(&desc);
      // FIXME: Add glfw error_code
      return std::make_pair(
        SRX_STATUS_MSG(EStatusCode::Not_Available,
                       "[GLFW] Window creation failed ({}): {}",
                       code,
                       desc),
        nullptr);
    }

    glfwMakeContextCurrent(window);

    if (!mMainWindow)
    {
      if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
      {
        const char* desc;
        const int   code = glfwGetError(&desc);
        glfwDestroyWindow(window);
        // FIXME: Add glfw error_code
        return std::make_pair(
          SRX_STATUS_MSG(EStatusCode::Not_Available,
                         "[GLFW] Window creation failed ({}): {}",
                         code,
                         desc),
          nullptr);
      }
      mMainWindow = window;
    }

    return std::make_pair(SRX_OK, window);
  }

  void DesktopGraphicsFramework::DestroyWindow(GLFWwindow* window) SRX_NOEXCEPT
  {
    if (window == nullptr)
      return;

    if (window == mMainWindow)
    {
      // Shutdown();
      mMainWindow = nullptr;
      if (mDirector)
        mDirector->Exit();
    }

    glfwDestroyWindow(window);
  }


  void DesktopGraphicsFramework::Update(const float deltaTime)
  {
    glfwPollEvents();
    if (mMainWindow && glfwWindowShouldClose(mMainWindow))
    {
      if (mDirector)
        mDirector->Exit();
    }
  }

  void DesktopGraphicsFramework::OnFinishFrame()
  {
    // TODO: Check if needed for active/all windows
    if (mMainWindow)
      glfwSwapBuffers(mMainWindow);
  }
}  // namespace
