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

#include "SxFileUtils.h"

namespace
{
  using namespace Sorex;

#if defined(_MSC_VER)
#  ifdef RSIZE_MAX
  constexpr size_t kErrorMaxSize = (size_t)RSIZE_MAX;
#  else
  constexpr size_t kErrorMaxSize = (size_t)(SIZE_MAX >> 1);
#  endif
#endif

  int GetFileMode(EAccessMode      mode,
                  File::EOpenMode  flags,
                  TArray<char, 4>& outMode) SRX_NOEXCEPT
  {
    if (mode == EAccessMode::None)
      return 0;

    int pos = 0;
    outMode.fill(0);

    const bool bCreate = Utils::CheckBitmask(flags, File::EOpenMode::Create);
    if (Utils::CheckBitmask(flags, File::EOpenMode::Append))
    {
      SRX_CHECK_MSG(bCreate, "Create flag is expected");
      outMode[pos++] = 'a';
    }
    else
    {
      if (mode == EAccessMode::Read && !bCreate)
        outMode[pos++] = 'r';
      else
        outMode[pos++] = 'w';
    }

    if (bCreate || mode == EAccessMode::ReadWrite)
      outMode[pos++] = '+';

    if (Utils::CheckBitmask(flags, File::EOpenMode::Binary))
      outMode[pos++] = 'b';

    return pos;
  }
}

namespace Ruffy::FileUtils
{
  FILE* OpenFile(StringView      path,
                 EAccessMode     mode,
                 File::EOpenMode flags,
                 Status*         status) SRX_NOEXCEPT
  {
    TArray<char, 4> fm;
    if (!GetFileMode(mode, flags, fm))
    {
      if (status)
        *status = SRX_STATUS_MSG(EStatusCode::Invalid_Argument,
                                 "file '{}' invalid open options",
                                 path);
      return nullptr;
    }

    FILE* file = nullptr;
    int   errc = 0;

#if defined(_MSC_VER)
    errc = fopen_s(&file, path.data(), fm.data());
#else
    file = fopen(path.data(), fm.data());
    errc = file == nullptr ? errno : 0;
#endif

    if (!file || errc)
    {
      RFY_MAKE_EXTERR(error,
                      errc,
                      "file '{}' opening failed: {}",
                      path,
                      GetErrorMessage(errc));
      return nullptr;
    }

    return file;
  }

  void CloseFile(fstream_t* file)
  {
    fclose(file);
  }

  void GetError(fstream_t* file, const char* ctx, Error* error)
  {
    if (error == nullptr)
      return;

    if (file == nullptr)
    {
      RFY_MAKE_ERR(error,
                   Error::Invalid_State,
                   "Invalid file stream (FILE) object");
      return;
    }

    errno_t errc = ferror(file);
    if (errc == 0)
    {
      error->Reset();
      return;
    }

    if (ctx)
      RFY_MAKE_EXTERR(error,
                      errc,
                      "file operation `{}` error: {}",
                      ctx,
                      GetErrorMessage(errc));
    else
      RFY_MAKE_EXTERR(error, errc, "{}", GetErrorMessage(errc));
  }


  String GetErrorMessage(errno_t errc)
  {
    if (errc == 0)
      return Error::ToString(Error::No_Error);

    String errmsg;

#if defined(_MSC_VER)
    constexpr int    n     = 1024;
    constexpr size_t bufsz = n < kErrorMaxSize ? n : kErrorMaxSize;
    char             msgbuf[bufsz];
    strerror_s(msgbuf, sizeof(msgbuf), errc);
    errmsg.assign(msgbuf);
#else
    if (const char* err = strerror(errc))
      errmsg.assign(err);
#endif

    RFY_ASSERT(errmsg.empty() == false);
    return errmsg;
  }
}
