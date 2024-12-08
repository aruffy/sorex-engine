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

#include "InputEvent.h"

namespace Sorex
{
  // @note: used as array index
  enum class EMouseButton
  {
    None     = -1,
    Button_1 = 0,
    Left     = Button_1,
    Button_2 = 1,
    Right    = Button_2,
    Button_3 = 2,
    Middle   = Button_3,
    Button_4 = 3,
    Button_5 = 4,
    Button_6 = 5,
    Button_7 = 6,
    Button_8 = 7,
  };

  class Mouse
  {
public:
    virtual ~Mouse() = default;

    virtual Point GetCursorPosition() const = 0;
    virtual Vec2  GetCursorMovement() const = 0;

    virtual bool IsButtonPressed(const EMouseButton button) const = 0;
  };

  enum class EMouseEvent
  {
    None,
    Move,
    Scroll,
    Button_Down,
    Button_Up,
  };

  class MouseEvent final: public InputEvent
  {
    SRX_RTTI(MouseEvent, InputEvent)

public:
    SRX_INLINE MouseEvent(Mouse* mouse, const Point& pos) SRX_NOEXCEPT;
    SRX_INLINE MouseEvent(Mouse* mouse, const Vec2& vec) SRX_NOEXCEPT;
    SRX_INLINE MouseEvent(Mouse*       mouse,
                          EMouseButton btn,
                          bool         bPress) SRX_NOEXCEPT;

    SRX_INLINE EMouseEvent GetMouseEventType() const { return mType; }
    EMouseButton           GetMouseButton() const SRX_NOEXCEPT;

    Point GetCursorPosition() const SRX_NOEXCEPT;
    Vec2  GetScroll() const SRX_NOEXCEPT;

    SRX_INLINE Mouse* GetMouse() const SRX_NOEXCEPT { return mMouse; }

private:
    Mouse*                              mMouse;
    EMouseEvent                         mType;
    TVariant<Point, Vec2, EMouseButton> mEventData;
  };

  SRX_INLINE MouseEvent::MouseEvent(Mouse* mouse, const Point& pos) SRX_NOEXCEPT
    : InputEvent(EInputEvent::Mouse)
    , mMouse(mouse)
    , mType(EMouseEvent::Move)
    , mEventData(Vec2(pos.x, pos.y))
  {}

  SRX_INLINE MouseEvent::MouseEvent(Mouse* mouse, const Vec2& vec) SRX_NOEXCEPT
    : InputEvent(EInputEvent::Mouse)
    , mMouse(mouse)
    , mType(EMouseEvent::Scroll)
    , mEventData(vec)
  {}

  SRX_INLINE MouseEvent::MouseEvent(Mouse*       mouse,
                                    EMouseButton btn,
                                    bool         bPress) SRX_NOEXCEPT

    : InputEvent(EInputEvent::Mouse)
    , mMouse(mouse)
    , mType(bPress ? EMouseEvent::Button_Down : EMouseEvent::Button_Up)
    , mEventData(btn)
  {}
}  // namespace
