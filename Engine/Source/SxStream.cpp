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

#include <Sorex/SxStream.h>

namespace Sorex
{
  bool Stream::Next(byte& value) SRX_NOEXCEPT
  {
    return Read({ &value, 1 }) == 1;
  }

  ssize_t Stream::ReadAll(TVector<byte>& buffer) SRX_NOEXCEPT
  {
    if (!Check(EAccessMode::Read))
      return SRX_READ_ERROR;

    const ssize_t length = GetLength();
    if (length == SRX_UNKNOWN_SIZE)
      return SRX_READ_ERROR;

    if (length == 0)
      return 0;

    const size_t n = buffer.size();
    buffer.resize(n + length);
    return Read({ &buffer[n], static_cast<size_t>(length) });
  }

  bool Stream::Write(const byte value) SRX_NOEXCEPT
  {
    return Write({ &value, 1 }) == 1;
  }

  ssize_t Stream::WriteAll(TSpan<const byte> buffer) SRX_NOEXCEPT
  {
    if (!Check(EAccessMode::Write))
      return SRX_WRITE_ERROR;

    if (buffer.empty())
      return 0;

    return Write({ buffer.data(), buffer.size() });
  }
}  // namespace
