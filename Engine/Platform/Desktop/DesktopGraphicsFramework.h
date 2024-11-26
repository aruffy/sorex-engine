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

struct GLFWwindow;
namespace Sorex::Platform
{
  /**
   * Class that based on Graphics Library Framework (glfw).
   * Create window and context for OpenGL.
   */
  class DesktopGraphicsFramework final
    : public Director::Component
    , private Director::IListener
  {
    SRX_RTTI(Platform::DesktopGraphicsFramework, Director::Component)

public:
    DesktopGraphicsFramework() = default;
    virtual ~DesktopGraphicsFramework() override;

    DesktopGraphicsFramework(const DesktopGraphicsFramework& other) = delete;
    DesktopGraphicsFramework& operator=(const DesktopGraphicsFramework& other) =
      delete;

    // Interface Director::Component
    virtual void   Attach(Director& director) override;
    virtual Status Initialize() override;
    virtual void   Shutdown() override;
    virtual void   Update(const float deltaTime) override;

    // Interface Director::IListener
    virtual void OnFinishFrame() override;


    /**
     * @brief Create new glfw window object.
     *
     * @param title - title of the window
     * @param size - desired window size
     * @param owner - real holder of glfw window object, will be associated with
     * window.
     * @param error - store error description if creation isn't done
     * successfully
     * @return The pointer to glfw window object or NULL if creation was't
     * successfully
     */
    TPair<Status, GLFWwindow*> CreateWindow(const WString& title,
                                            const SizeInt& size) SRX_NOEXCEPT;
    /**
     * @brief Destroy glfw window object.
     *
     * @param window - the window object pointer that will be destroyed
     */
    void DestroyWindow(GLFWwindow* window) SRX_NOEXCEPT;

private:
    bool        mIsInitialized = false;
    GLFWwindow* mMainWindow    = nullptr;

    Director* mDirector = nullptr;
  };
}
