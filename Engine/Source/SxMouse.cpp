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

#include <Sorex/Input/SxMouse.h>

namespace Sorex
{
  Point MouseEvent::GetCursorPosition() const SRX_NOEXCEPT
  {
    SRX_CHECK(mType == EMouseEvent::Move
              && std::holds_alternative<Point>(mEventData));

    if (mType != EMouseEvent::Scroll)
      return Point();

    const Point* const pos = std::get_if<Point>(&mEventData);
    return pos ? *pos : Point();
  }

  Vec2 MouseEvent::GetScroll() const SRX_NOEXCEPT
  {
    SRX_CHECK(mType == EMouseEvent::Scroll
              && std::holds_alternative<Vec2>(mEventData));

    if (mType != EMouseEvent::Scroll)
      return Vec2::Zero();

    const Vec2* const vec = std::get_if<Vec2>(&mEventData);
    return vec ? *vec : Vec2::Zero();
  }

  EMouseButton MouseEvent::GetMouseButton() const SRX_NOEXCEPT
  {
    SRX_CHECK(
      (mType == EMouseEvent::Button_Up || mType == EMouseEvent::Button_Down)
      && std::holds_alternative<EMouseButton>(mEventData));

    if (mType != EMouseEvent::Button_Up && mType != EMouseEvent::Button_Down)
      return EMouseButton::None;

    const EMouseButton* const btn = std::get_if<EMouseButton>(&mEventData);
    return btn ? *btn : EMouseButton::None;
  }

}  // namespace
