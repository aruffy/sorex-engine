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

#include <Sorex/CoreMinimal.h>
#include <Sorex/Director.h>
#include <Sorex/Containers/SxListenerContainer.h>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

namespace Sorex::Platform
{
  enum class EGraphicsFrameworkStatusCode
  {
    No_Error              = GLFW_NO_ERROR,
    Not_Initialized       = GLFW_NOT_INITIALIZED,
    No_Current_Contex     = GLFW_NO_CURRENT_CONTEXT,
    Invalid_Enum          = GLFW_INVALID_ENUM,
    Invalid_Value         = GLFW_INVALID_VALUE,
    Out_Of_Memory         = GLFW_OUT_OF_MEMORY,
    Api_Unavailable       = GLFW_API_UNAVAILABLE,
    Version_Unavailable   = GLFW_VERSION_UNAVAILABLE,
    Platform_Error        = GLFW_PLATFORM_ERROR,
    Format_Unavailable    = GLFW_FORMAT_UNAVAILABLE,
    No_Window_Context     = GLFW_NO_WINDOW_CONTEXT,
    Cursor_Unavailable    = GLFW_CURSOR_UNAVAILABLE,
    Feature_Unavailable   = GLFW_FEATURE_UNAVAILABLE,
    Feature_Unimplemented = GLFW_FEATURE_UNIMPLEMENTED,
    Platform_Unavailable  = GLFW_PLATFORM_UNAVAILABLE
  };
  namespace Details
  {
    class GraphicsFrameworkErrorCategory final: public std::error_category
    {
  public:
      typedef EGraphicsFrameworkStatusCode EStatusCode;

      const char* name() const noexcept override;
      std::string message(int errcode) const override;

      SRX_NODISCARD static SRX_INLINE std::error_code create(
        const EStatusCode errcode = static_cast<EStatusCode>(0)) SRX_NOEXCEPT
      {
        static const GraphicsFrameworkErrorCategory errorCategory;
        return std::error_code(static_cast<int>(errcode), errorCategory);
      }

  private:
      GraphicsFrameworkErrorCategory() = default;
    };
  }  // namespace Details
}  // namespace

namespace std
{
  template<>
  struct is_error_code_enum<Sorex::Platform::EGraphicsFrameworkStatusCode>
    : public true_type
  {};
}

struct GLFWwindow;
namespace Sorex::Platform
{
  inline std::error_code make_error_code(
    EGraphicsFrameworkStatusCode errcode) noexcept
  {
    return Details::GraphicsFrameworkErrorCategory::create(errcode);
  }

  /**
   * Class that based on Graphics Library Framework (glfw).
   * Create window and context for OpenGL.
   */
  class DesktopGraphicsFramework final: public Director::Component
  {
    SRX_RTTI(Platform::DesktopGraphicsFramework, Director::Component)

public:
    class IListener
    {
  public:
      /**
       * @brief Called when a new window is created.
       * @param window Reference to the created GLFWwindow.
       */
      virtual void OnWindowCreate(GLFWwindow& window) {};

      /**
       * @brief Called when a window is destroyed.
       * @param window Pointer to the GLFWwindow being destroyed. If null, all
       * windows will be destroyed.
       */
      virtual void OnWindowDestroy(GLFWwindow* window) {};

  protected:
      virtual ~IListener() = default;
    };

public:
    DesktopGraphicsFramework() = default;
    virtual ~DesktopGraphicsFramework() override;

    DesktopGraphicsFramework(const DesktopGraphicsFramework& other) = delete;
    DesktopGraphicsFramework& operator=(const DesktopGraphicsFramework& other) =
      delete;

    // Interface Director::Component
    virtual Status Initialize() override;
    virtual void   Shutdown() override;
    virtual void   Update(const float deltaTime) override;

    /**
     * @brief Create new glfw window object.
     *
     * @param title - utf8 string to name the window;
     * @param size - desired window size, if null the full screen window;
     * @param pos - position of new window;
     */
    TPair<Status, GLFWwindow*> CreateWindow(const WStringView& title,
                                            const SizeInt*     size = nullptr,
                                            const PointInt*    pos  = nullptr)
      SRX_NOEXCEPT;
    /**
     * @brief Destroy glfw window object.
     *
     * @param window - the window object pointer that will be destroyed
     */
    void DestroyWindow(GLFWwindow* window) SRX_NOEXCEPT;

    // Listeners
    SRX_INLINE bool AddListener(IListener* listener) SRX_NOEXCEPT
    {
      return mListeners.Add(listener);
    }
    SRX_INLINE void RemoveListener(IListener* listener) SRX_NOEXCEPT
    {
      mListeners.Remove(listener);
    }

    SRX_INLINE GLFWwindow* GetMainWindow() const { return mMainWindow; }

private:
    bool        mIsInitialized = false;
    GLFWwindow* mMainWindow    = nullptr;

    TListenerContainer<IListener> mListeners;
  };
}
