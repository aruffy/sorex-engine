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

#include <Sorex/Win32Platform.h>

#include <cstdio>

#include <Sorex/Assert.h>
#include <Sorex/Platform.h>

#include <Windows.h>

#ifdef _DEBUG
#  include <crtdbg.h>
#  include <debugapi.h>
#endif

namespace
{
#ifdef _DEBUG
  bool ShowQuitMessage(const char* message,
                       const char* file,
                       int         line) SRX_NOEXCEPT
  {
    MSG  msg;
    BOOL bQuit   = PeekMessage(&msg, NULL, WM_QUIT, WM_QUIT, PM_REMOVE);
    BOOL bResult = _CrtDbgReport(_CRT_ASSERT, file, line, NULL, "%s", message);

    if (bQuit)
      PostQuitMessage((int)msg.wParam);

    return bResult == 0;
  }
#endif
}

namespace Sorex::Platform
{
  int OnAssertionFailed(const char* message,
                        const char* file,
                        int         line) SRX_NOEXCEPT
  {
#ifdef _DEBUG
    if (!ShowQuitMessage(message, file, line))
      ::DebugBreak();
#endif

    return 0;
  }

  void OnCheckFailed(const char* message,
                     const char* file,
                     int         line) SRX_NOEXCEPT
  {
#ifdef _DEBUG
    ShowQuitMessage(message, file, line);
#else
    if (message && file)
    {
      fprintf(stderr,
              "[FATAL] %s:%i Check failed: '%s'\n",
              file,
              line,
              message);
      fflush(stderr);
    }
#endif
  }
}  // namespace
