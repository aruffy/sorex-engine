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

#include <Sorex/FileSystem/SxFile.h>

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

  String GetFileMode(EAccessMode mode, File::EOpenMode flags) SRX_NOEXCEPT
  {
    if (mode == EAccessMode::None)
      return 0;

    String     res;
    const bool bCreate = Utils::CheckBitmask(flags, File::EOpenMode::Create);
    if (Utils::CheckBitmask(flags, File::EOpenMode::Append))
    {
      SRX_CHECK_MSG(bCreate, "Create flag is expected");
      res.push_back('a');
    }
    else
    {
      if (mode == EAccessMode::Read && !bCreate)
        res.push_back('r');
      else
        res.push_back('w');
    }

    if (bCreate || mode == EAccessMode::ReadWrite)
      res.push_back('+');

    if (Utils::CheckBitmask(flags, File::EOpenMode::Binary))
      res.push_back('b');

    return res;
  }

  FILE* OpenFile(StringView      path,
                 EAccessMode     access,
                 File::EOpenMode mode,
                 Status*         status) SRX_NOEXCEPT
  {
    const String filemode = GetFileMode(access, mode);
    if (filemode.empty())
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
    errc = fopen_s(&file, path.data(), filemode.c_str());
#else
    file = fopen(path.data(), filemode.c_str());
    errc = file == nullptr ? errno : 0;
#endif

    if (errc || !file)
    {
      if (status)
        *status = Status(std::error_code(errc, std::generic_category()));

      if (file)
      {
        SRX_NOENTRY("invalid errno handling");
        fclose(file);
      }

      return nullptr;
    }

    return file;
  }
}

namespace Sorex
{
  TUniquePointer<File> File::Open(StringView  path,
                                  EAccessMode access,
                                  EOpenMode   mode,
                                  Status*     status) SRX_NOEXCEPT
  {
    if (FILE* const f = OpenFile(path, access, mode, status))
      return TUniquePointer<File>(new File(f, path, access, mode));

    return nullptr;
  }

  File::File(StringView  path,
             EAccessMode access /* = EAccessMode::Read */,
             EOpenMode   mode /* = EileOpenMode::Binary */) SRX_NOEXCEPT
    : mPath(path)
    , mAccess(access)
    , mFile(nullptr)
    , mTotalLength(SRX_UNKNOWN_SIZE)
  {
    Status status;
    if (FILE* const f = OpenFile(path, access, mode, &status))
      mFile = f;
    else
      mStatus = std::move(status);
  }

  File::File(FILE* file, StringView path, EAccessMode access, EOpenMode mode)
    SRX_NOEXCEPT
    : mPath(path)
    , mAccess(access)
    , mFile(file)
    , mTotalLength(SRX_UNKNOWN_SIZE)
  {
    SRX_CHECK(file);
  }

  File::~File()
  {
    if (mFile)
      fclose(mFile);
  }

  bool File::Check(const EAccessMode mode) const SRX_NOEXCEPT
  {
    if (mode == EAccessMode::None && !IsOpen())
      return false;

    return Utils::CheckBitmask(mAccess, mode);
  }

  bool File::Flush() SRX_NOEXCEPT
  {
    const bool bWritable = Check(EAccessMode::Write);
    if (!bWritable)
    {
      SRX_NOENTRY("File invalid or operation not allowed");
      mStatus = SRX_STATUS_MSG(EStatusCode::Not_Permitted,
                               "Write operation not allowed");
      return false;
    }

    if (int errcode = fflush(mFile); errcode != 0)
    {
      mStatus = Status(std::error_code(errcode, std::generic_category()));
      return false;
    }

    return true;
  }

  bool File::EndOfFile() const SRX_NOEXCEPT
  {
    return mFile && (feof(mFile) != 0);
  }

  ssize_t File::GetLength() const SRX_NOEXCEPT
  {
    SRX_CHECK(IsOpen());

    const auto pos = ftell(mFile);
    if (pos < 0)
    {
      mStatus = Status(std::error_code(errno, std::generic_category()));
      return SRX_UNKNOWN_SIZE;
    }

    if (mTotalLength == SRX_UNKNOWN_SIZE)
    {
      if (fseek(mFile, 0, SEEK_END) != 0)
        return SRX_UNKNOWN_SIZE;

      if (const auto len = ftell(mFile); len < 0)
        mStatus = Status(std::error_code(errno, std::generic_category()));
      else
        mTotalLength = static_cast<ssize_t>(len);

      SRX_VERIFY(fseek(mFile, pos, SEEK_SET) == 0);
    }

    return mTotalLength - pos;
  }

  ssize_t File::GetPosition() const SRX_NOEXCEPT
  {
    SRX_CHECK(IsOpen());

    const auto position = ftell(mFile);
    if (position < 0)
    {
      mStatus = Status(std::error_code(errno, std::generic_category()));
      return SRX_UNKNOWN_SIZE;
    }

    return position;
  }

  bool File::Seek(int32 pos, ESeekMode mode) SRX_NOEXCEPT
  {
    SRX_CHECK(IsOpen());
    if (!IsOpen())
      return false;

    int m;
    switch (mode)
    {
    case ESeekMode::Begin:
      m = SEEK_SET;
      break;
    case ESeekMode::Current:
      m = SEEK_CUR;
      break;
    case ESeekMode::End:
      m = SEEK_END;
      break;
    default:
      return false;
    }

    return fseek(mFile, pos, m) == 0;
  }

  bool File::Peek(byte& value) SRX_NOEXCEPT
  {
    mStatus = SRX_STATUS_MSG(EStatusCode::Not_Implemented, "Peek()");
    return false;
  }

  ssize_t File::Read(TSpan<byte> buffer, ssize_t length) SRX_NOEXCEPT
  {
    if (!Check(EAccessMode::Read))
    {
      SRX_NOENTRY("File invalid or operation not allowed");
      mStatus = SRX_STATUS_MSG(EStatusCode::Invalid_State,
                               "Read() operation not allowed");
      return SRX_READ_ERROR;
    }

    const size_t toRead = (length == SRX_UNKNOWN_SIZE)
                            ? buffer.size()
                            : std::min<size_t>(buffer.size(), length);

    const auto count = fread(buffer.data(), 1, toRead, mFile);
    if (count == toRead || !ferror(mFile))
      return static_cast<ssize_t>(count);

    mStatus = SRX_STATUS_MSG(EStatusCode::Bad_File, "Read() failed");
    return SRX_READ_ERROR;
  }

  ssize_t File::Write(TSpan<const byte> buffer) SRX_NOEXCEPT
  {
    if (buffer.empty())
      return 0;

    if (!Check(EAccessMode::Write))
    {
      mStatus = SRX_STATUS_MSG(EStatusCode::Invalid_State,
                               "Write() operation not allowed");
      return SRX_WRITE_ERROR;
    }

    const auto count = fwrite(buffer.data(), 1, buffer.size(), mFile);
    mTotalLength += count;

    if (count != buffer.size())
    {
      mStatus = SRX_STATUS_MSG(EStatusCode::Bad_File, "Write() failed");
      return SRX_WRITE_ERROR;
    }

    return count;
  }

  bool File::Reset() SRX_NOEXCEPT
  {
    if (!IsOpen())
      return true;

    if (!Seek(0, ESeekMode::Begin))
      return false;

    Stream::Reset();
    return true;
  }
}  // namespace
