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
#include <Sorex/Stream.h>

namespace Sorex
{
  class File final: public Stream
  {
    SRX_RTTI(File, Stream)

public:
    enum class EOpenMode : uint8
    {
      Binary = (1 << 0),
      Append = (1 << 1),
      Create = (1 << 2),
      SRX_ENUM_BITMASK
    };

public:
    static TUniquePointer<File> Open(StringView  path,
                                     EAccessMode access = EAccessMode::Read,
                                     EOpenMode   mode   = EOpenMode::Binary,
                                     Status*     status = nullptr) SRX_NOEXCEPT;

public:
    explicit File(StringView  path,
                  EAccessMode access = EAccessMode::Read,
                  EOpenMode   mode   = EOpenMode::Binary) SRX_NOEXCEPT;

    virtual ~File() override;

    File(const File& other)            = delete;
    File& operator=(const File& other) = delete;

    bool Flush() SRX_NOEXCEPT;

    // Stream API
    virtual bool Check(const EAccessMode mode) const SRX_NOEXCEPT override;

    virtual bool       IsOpen() const SRX_NOEXCEPT override { return mFile; }
    virtual StringView GetName() const SRX_NOEXCEPT override { return mPath; }

    virtual bool    EndOfFile() const SRX_NOEXCEPT override;
    virtual ssize_t GetLength() const SRX_NOEXCEPT override;
    virtual ssize_t GetPosition() const SRX_NOEXCEPT override;


    virtual bool Seek(int32 pos, ESeekMode mode) SRX_NOEXCEPT override;
    virtual bool Peek(byte& value) SRX_NOEXCEPT override;

    virtual ssize_t Read(TSpan<byte> buffer, ssize_t length = SRX_UNKNOWN_SIZE)
      SRX_NOEXCEPT override;
    virtual ssize_t Write(TSpan<const byte> buffer) SRX_NOEXCEPT override;

    virtual bool Reset() SRX_NOEXCEPT override;

private:
    File(FILE* file, StringView path, EAccessMode access, EOpenMode mode)
      SRX_NOEXCEPT;

private:
    String      mPath;
    EAccessMode mAccess;

    FILE*           mFile;
    mutable ssize_t mTotalLength;  ///< Cached total length
  };
}  // namespace

using SxFile = Sorex::File;
