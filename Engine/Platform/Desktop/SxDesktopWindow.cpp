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

#include "SxDesktopWindow.h"

namespace Sorex::Platform
{
  DesktopWindow::DesktopWindow(DesktopGraphicsFramework& glfw,
                               const WStringView         title,
                               SizeInt                   size) SRX_NOEXCEPT
    : mGlfw(glfw)
    , mTitle(title)
    , mSize(size)
    , mWindow(nullptr)
    , mDirector(nullptr)
  {}

  Status DesktopWindow::Initialize()
  {
    SRX_CLSFUN_TRACE();
    SRX_CHECK(mSize.IsValid() && !mTitle.empty());

    auto [status, window] = mGlfw.CreateWindow(mTitle, &mSize);
    if (!status.Ok())
      return status;

    glfwSetWindowUserPointer(window, this);

    mWindow = window;
    return SRX_OK;
  }

  void DesktopWindow::Shutdown()
  {
    SRX_CLSFUN_TRACE();

    if (mDirector)
    {
      mDirector->RemoveListener(this);
      mDirector = nullptr;
    }

    if (mWindow)
    {
      mGlfw.DestroyWindow(mWindow);
      mWindow = nullptr;
    }
  }

  void DesktopWindow::Attach(Director& director)
  {
    SRX_CHECK(!mDirector);

    Director::Component::Attach(director);

    director.AddListener(this);
    mDirector = &director;
  }

  void DesktopWindow::Update(const float deltaTime)
  {
    if (mWindow && glfwWindowShouldClose(mWindow))
      Shutdown();
  }

  void DesktopWindow::OnFinishFrame()
  {
    SRX_CHECK(mWindow);

    if (mWindow)
      glfwSwapBuffers(mWindow);
  }
}  // namespace
