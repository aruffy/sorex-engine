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

#include <Sorex/Input/SxMouse.h>

#include "DesktopGraphicsFramework.h"

namespace Sorex::Platform
{
  class DesktopMouse final: public Mouse
  {
    static constexpr int kButtonNumber =
      static_cast<int>(EMouseButton::Button_8) + 1;

public:
    SRX_INLINE explicit DesktopMouse(DesktopGraphicsFramework& glfw)
      SRX_NOEXCEPT: mGlfw(glfw)
    {}

    // Interface Mouse
    virtual Point GetCursorPosition() const override;
    virtual Vec2  GetCursorMovement() const override;
    virtual bool  IsButtonPressed(const EMouseButton button) const override;

    SRX_INLINE void SetCursorPosition(const Point& pos) SRX_NOEXCEPT;

private:
    Vec2 mPosition;
    Vec2 mPrevPosition;

    DesktopGraphicsFramework& mGlfw;
  };

  SRX_INLINE void DesktopMouse::SetCursorPosition(const Point& pos) SRX_NOEXCEPT
  {
    mPrevPosition = mPosition;
    mPosition.x   = pos.x;
    mPosition.y   = pos.y;
  }
}
