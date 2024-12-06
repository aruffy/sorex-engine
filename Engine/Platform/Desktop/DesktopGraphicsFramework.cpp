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

#include <Sorex/Utils/String.h>

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
  namespace Details
  {
    const char* GraphicsFrameworkErrorCategory::name() const noexcept
    {
      return "GLFW";
    }

    std::string GraphicsFrameworkErrorCategory::message(int errcode) const
    {
      switch (errcode)
      {
      case GLFW_NO_ERROR:
        [[unlikely]] return "No Error";
      case GLFW_NOT_INITIALIZED:
        return "The GLFW library is not initialized";
      case GLFW_NO_CURRENT_CONTEXT:
        return "There is no current context";
      case GLFW_INVALID_ENUM:
        return "Invalid argument for enum parameter";
      case GLFW_INVALID_VALUE:
        return "Invalid value for parameter";
      case GLFW_OUT_OF_MEMORY:
        return "Out of memory";
      case GLFW_API_UNAVAILABLE:
        return "The requested API is unavailable";
      case GLFW_VERSION_UNAVAILABLE:
        return "The requested API version is unavailable";
      case GLFW_PLATFORM_ERROR:
        return "A platform-specific error occurred";
      case GLFW_FORMAT_UNAVAILABLE:
        return "The requested format is unavailable";
      case GLFW_NO_WINDOW_CONTEXT:
        return "The specified window has no context";
      case GLFW_CURSOR_UNAVAILABLE:
        return "The specified cursor shape is unavailable";
      case GLFW_FEATURE_UNAVAILABLE:
        return "The requested feature cannot be implemented for this platform";
      case GLFW_FEATURE_UNIMPLEMENTED:
        return "The requested feature has not yet been implemented for this "
               "platform";
      case GLFW_PLATFORM_UNAVAILABLE:
        return "The requested platform is unavailable";
      default:
        [[unlikely]] return "Unknown Error Code";
      }
    }
  }  // namespace Details

  DesktopGraphicsFramework::~DesktopGraphicsFramework()
  {
    // Shutdown();
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
      return SRX_STATUS_MSG(static_cast<EGraphicsFrameworkStatusCode>(code),
                            "[GLFW] Initialization failed ({}): {}",
                            code,
                            desc);
    }

    // Callbacks
    glfwSetErrorCallback(OnFrameworkError);
    mIsInitialized = true;

    return SRX_OK;
  }

  void DesktopGraphicsFramework::Shutdown()
  {
    SRX_CLSFUN_TRACE();
    if (!mIsInitialized)
      return;

    if (mMainWindow)
    {
      for (IListener* listener : mListeners)
        listener->OnWindowDestroy(nullptr);

      mMainWindow = nullptr;
    }

    glfwTerminate();  // NOTE: Should destroy all windows
    mIsInitialized = false;
  }

  TPair<Status, GLFWwindow*> DesktopGraphicsFramework::CreateWindow(
    const WStringView& title,
    const SizeInt*     size /* = nullptr */,
    const PointInt*    pos /* = nullptr */) SRX_NOEXCEPT
  {
    SRX_CLSFUN_TRACE();
    SRX_CHECK(mIsInitialized);

    SizeInt      wSize;
    GLFWmonitor* monitor     = NULL;
    int          redBits     = 8;
    int          greenBits   = 8;
    int          blueBits    = 8;
    int          alphaBits   = 8;
    int          depthBits   = 24;
    int          stencilBits = 8;

    if (mMainWindow)
      glfwDefaultWindowHints();

    if (size && size->IsValid())
    {
      wSize = *size;
      glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);
      glfwWindowHint(GLFW_DECORATED, GL_TRUE);

      if (pos)
      {
        glfwWindowHint(GLFW_POSITION_X, pos->x);
        glfwWindowHint(GLFW_POSITION_Y, pos->y);
      }
    }
    else
    {
      monitor = glfwGetPrimaryMonitor();
      if (const GLFWvidmode* mode = glfwGetVideoMode(monitor))
      {
        redBits      = mode->redBits;
        greenBits    = mode->greenBits;
        blueBits     = mode->blueBits;
        wSize.width  = mode->width;
        wSize.height = mode->height;

        glfwWindowHint(GLFW_REFRESH_RATE, mode->refreshRate);
      }

      glfwWindowHint(GLFW_DECORATED, GL_FALSE);
      glfwWindowHint(GLFW_CENTER_CURSOR, GL_TRUE);
      glfwWindowHint(GLFW_FOCUS_ON_SHOW, GL_TRUE);
    }

#ifdef SOREX_PLATFORM_MACOSX
    glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
#else
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_API);
#endif

    // Context specific hints
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    glfwWindowHint(GLFW_RED_BITS, redBits);
    glfwWindowHint(GLFW_GREEN_BITS, greenBits);
    glfwWindowHint(GLFW_BLUE_BITS, blueBits);
    glfwWindowHint(GLFW_ALPHA_BITS, alphaBits);
    glfwWindowHint(GLFW_DEPTH_BITS, depthBits);
    glfwWindowHint(GLFW_STENCIL_BITS, stencilBits);

#ifdef SOREX_DEBUG_MEDIUM
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GL_TRUE);
#endif

    const String wTitle =
      !title.empty() ? Utils::ToUtf8String(title) : String("Sorex Window");
    GLFWwindow* window = glfwCreateWindow(wSize.width,
                                          wSize.height,
                                          wTitle.c_str(),
                                          monitor,
                                          NULL);
    if (window == nullptr)
    {
      const char* desc;
      int         code = glfwGetError(&desc);
      return std::make_pair(
        SRX_STATUS_MSG(static_cast<EGraphicsFrameworkStatusCode>(code),
                       "[GLFW] Window creation failed: {}",
                       desc),
        nullptr);
    }

    glfwMakeContextCurrent(window);
    if (!mMainWindow)
    {
      SRX_DEBUG("[DesktopGraphicsFramework] Create main window: loading OpenGL "
                "functions.");

      // cppcheck-suppress cstyleCast
      if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
      {
        const char* desc;
        glfwGetError(&desc);
        glfwDestroyWindow(window);

        return std::make_pair(
          SRX_STATUS_MSG(
            EStatusCode::Not_Available,
            "[DesktopGraphicsFramework] Loading OpenGL functions failed: {}",
            desc),
          nullptr);
      }
      mMainWindow = window;
    }

    for (IListener* listener : mListeners)
      listener->OnWindowCreate(*window);

    return std::make_pair(SRX_OK, window);
  }

  void DesktopGraphicsFramework::DestroyWindow(GLFWwindow* window) SRX_NOEXCEPT
  {
    SRX_CLSFUN_TRACE();

    if (window == nullptr)
      return;

    for (IListener* listener : mListeners)
      listener->OnWindowCreate(*window);

    if (window == mMainWindow)
    {
      mMainWindow = nullptr;
      if (auto director = GetDirector())
        director->Exit();
    }

    glfwDestroyWindow(window);
  }

  void DesktopGraphicsFramework::Update(const float deltaTime)
  {
    glfwPollEvents();
  }
}  // namespace
