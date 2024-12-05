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
#include <Sorex/Window.h>

namespace Sorex
{
  class IInputEventHandler
  {
public:
    virtual ~IInputEventHandler() = default;

    virtual void OnMouseEvent(Window* window) {}
    virtual void OnTouchEvent() {}

    virtual void OnKeyboardEvent(Window* window) {};

    virtual void OnWindowEvent() {};
    virtual void OnFocusEvent() {};
  };

  class InputSystem: public Director::Component
  {
    SRX_RTTI(InputSystem, Director::Component)

public:
    InputSystem()                   = default;
    virtual ~InputSystem() override = default;

    InputSystem(const InputSystem& other)            = delete;
    InputSystem& operator=(const InputSystem& other) = delete;

    // virtual Keyboard* GetKeyboard() = 0;
    // virtual Mouse* GetMouse() = 0;
    // virtual TouchScreen* GetTouchScreen() = 0;
private:
  };
}
