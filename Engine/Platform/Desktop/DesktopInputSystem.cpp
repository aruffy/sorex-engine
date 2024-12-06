/**************************************************************************/
/*                         This file is part of:                          */
/*                                SOREX                                   */
/*                 Simple OpenGL Rendering Engine eXtended                */
/**************************************************************************/
/* Copyright (c) 2022-2024 Aleksandr Ershov (Ruffy).                      */
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

#include "DesktopInputSystem.h"

namespace
{
  using namespace Sorex;
  Platform::DesktopInputSystem* inputSystem = nullptr;

  void OnMouseClickCallback(GLFWwindow* window,
                            int         button,
                            int         action,
                            int         modify) SRX_NOEXCEPT
  {
    SRX_CHECK(inputSystem);
    SRX_DEBUG("Mouse Click {}", button);
  }

  void OnMouseMoveCallback(GLFWwindow* window, double x, double y) SRX_NOEXCEPT
  {
    SRX_CHECK(inputSystem);
    SRX_DEBUG("Mouse Move {}x{}", x, y);
  }

  void OnMouseScrollCallback(GLFWwindow* window,
                             double      x,
                             double      y) SRX_NOEXCEPT
  {
    SRX_CHECK(inputSystem);
    SRX_DEBUG("Mouse Scroll {}x{}", x, y);
  }
}  // namespace

namespace Sorex::Platform
{
  DesktopInputSystem::DesktopInputSystem(DesktopGraphicsFramework& glfw)
    SRX_NOEXCEPT: mGlfw(glfw)
  {
    SRX_CHECK(inputSystem == nullptr);

    inputSystem = this;
    mGlfw.AddListener(this);
  }

  DesktopInputSystem::~DesktopInputSystem()
  {
    mGlfw.RemoveListener(this);
    inputSystem = nullptr;
  }

  Status DesktopInputSystem::Initialize()
  {
    SRX_CLSFUN_TRACE();

    return SRX_OK;
  }
  void DesktopInputSystem::Shutdown()
  {
    SRX_CLSFUN_TRACE();
  };

  void DesktopInputSystem::OnWindowCreate(GLFWwindow& window)
  {
    /* Mouse callbacks */
    glfwSetMouseButtonCallback(&window, OnMouseClickCallback);
    glfwSetCursorPosCallback(&window, OnMouseMoveCallback);
    glfwSetScrollCallback(&window, OnMouseScrollCallback);
  }
}  // namespace
