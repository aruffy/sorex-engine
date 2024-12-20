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

#include <Sorex/Utils/SxString.h>

namespace Sorex::Utils
{
  SRX_API bool StartWith(StringView str, StringView prefix) SRX_NOEXCEPT
  {
    if (prefix.size() > str.size())
      return false;

    if (prefix.empty())
      return true;

    return (str.rfind(prefix, 0) == 0);
  }

  SRX_API String ToUtf8String(WStringView wstr) SRX_NOEXCEPT
  {
    if (wstr.empty())
      return String();

    String s;
    s.reserve(wstr.length());

    for (size_t i = 0; i < wstr.length(); ++i)
    {
      const uint32 ch  = wstr[i];
      const uint32 val = static_cast<uint32>(ch);
      if (val < 0x0080U)
      {
        s += (char)ch;
      }
      else if (val < 0x0800U)
      {
        s += (char)(0xC0 | ((ch >> 6) & 0x1F));
        s += (char)(0x80 | (ch & 0x3F));
      }
      else if (val < 0x10000U)
      {
        s += (char)(0xE0 | ((ch >> 12) & 0x0F));
        s += (char)(0x80 | ((ch >> 6) & 0x3F));
        s += (char)(0x80 | (ch & 0x3F));
      }
      else
      {
        s += (char)(0xF0 | ((ch >> 18) & 0x07));
        s += (char)(0x80 | ((ch >> 12) & 0x3F));
        s += (char)(0x80 | ((ch >> 6) & 0x3F));
        s += (char)(0x80 | (ch & 0x3F));
      }
    }

    return s;
  }

}  // namespace
