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

#include "SxDesktopInputSystem.h"

namespace Sorex::Platform
{
  DesktopInputSystem* DesktopInputSystem::inputSystem = nullptr;

  TUniquePointer<DesktopInputSystem> DesktopInputSystem::Create(
    DesktopGraphicsFramework& glfw) SRX_NOEXCEPT
  {
    SRX_CHECK(inputSystem == nullptr);
    if (inputSystem)
      return nullptr;

    return TUniquePointer<DesktopInputSystem>(new DesktopInputSystem(glfw));
  }

  DesktopInputSystem::DesktopInputSystem(DesktopGraphicsFramework& glfw)
    SRX_NOEXCEPT
    : mGlfw(glfw)
    , mMouse(glfw)
  {
    SRX_ASSERT(!inputSystem);

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

  void DesktopInputSystem::OnMouseClickCallback(GLFWwindow* window,
                                                int         button,
                                                int         action,
                                                int         modify) SRX_NOEXCEPT
  {
    SRX_CHECK(inputSystem);
    SRX_DEBUG("[{}] Mouse Click: {}",
              inputSystem->GetRuntimeClass().GetName(),
              button);

    if (inputSystem->mListeners.IsEmpty())
      return;

    constexpr int kBtn1 = (int)EMouseButton::Button_1;
    constexpr int kBtn8 = (int)EMouseButton::Button_8;
    static_assert(kBtn1 == GLFW_MOUSE_BUTTON_1 && kBtn8 == GLFW_MOUSE_BUTTON_8);

    const bool isValidButtonId = (button >= kBtn1 && button <= kBtn8);
    SRX_CHECK(isValidButtonId);
    if (isValidButtonId)
    {
      const MouseEvent event(&inputSystem->mMouse,
                             static_cast<EMouseButton>(button),
                             action == GLFW_PRESS);

      Window* const wptr =
        static_cast<Window*>(glfwGetWindowUserPointer(window));

      for (auto listener : inputSystem->mListeners)
        listener->OnMouseEvent(wptr, event);
    }
  }

  void DesktopInputSystem::OnMouseMoveCallback(GLFWwindow* window,
                                               double      x,
                                               double      y) SRX_NOEXCEPT
  {
    SRX_CHECK(inputSystem);

    const Point pos(static_cast<scalar_t>(x), static_cast<scalar_t>(y));
    inputSystem->mMouse.SetCursorPosition(pos);

    if (inputSystem->mListeners.IsEmpty())
      return;

    Window* const wptr = static_cast<Window*>(glfwGetWindowUserPointer(window));
    const MouseEvent event(&inputSystem->mMouse, pos);
    SRX_CHECK(event.GetMouseEventType() == EMouseEvent::Move);

    for (auto listener : inputSystem->mListeners)
      listener->OnMouseEvent(wptr, event);
  }

  void DesktopInputSystem::OnMouseScrollCallback(GLFWwindow* window,
                                                 double      x,
                                                 double      y) SRX_NOEXCEPT
  {
    SRX_CHECK(inputSystem);

    if (inputSystem->mListeners.IsEmpty())
      return;

    Window* const wptr = static_cast<Window*>(glfwGetWindowUserPointer(window));
    const Vec2    scroll(static_cast<scalar_t>(x), static_cast<scalar_t>(y));
    const MouseEvent event(&inputSystem->mMouse, scroll);
    SRX_CHECK(event.GetMouseEventType() == EMouseEvent::Scroll);

    for (auto listener : inputSystem->mListeners)
      listener->OnMouseEvent(wptr, event);
  }

}  // namespace
