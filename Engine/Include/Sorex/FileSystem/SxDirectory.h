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

#include <Sorex/SxCoreMinimal.h>

#include "SxFileSystem.h"

namespace Sorex::FileSystem
{
  class Directory: public IFileSystem
  {
public:
    explicit Directory(PathStringView path,
                       IFileSystem*   parent = nullptr) SRX_NOEXCEPT;

    virtual ~Directory() override {}

    virtual Path GetSystemPath() const SRX_NOEXCEPT override;

    const Path&  GetBasePath() const { return mBasepath; }
    IFileSystem* GetParent() const { return mParent; }

protected:
    struct Catalog
    {
      PathString         name;
      TVector<FileIndex> files;
    };

private:
    Path mBasepath;  ///< Parent related system path in a in a platform
                     ///< preffered format
    IFileSystem* mParent;
  };

  class StaticDirectory final: public Directory
  {
public:
    // @TODO: Path
    explicit StaticDirectory(PathStringView path,
                             IFileSystem*   parent = nullptr);

    virtual Status Mount(const Path& path) SRX_NOEXCEPT override;
    virtual Status IndexFiles() SRX_NOEXCEPT override;

    virtual void GetFiles(PathStringView      path,
                          TVector<FileIndex>& files) SRX_NOEXCEPT override;

    virtual TPair<EFileStatus, TOptional<FileIndex>> GetFile(
      PathStringView path) const SRX_NOEXCEPT override;

    virtual TUniquePointer<Stream> OpenFile(const FileIndex& fileIndex,
                                            Status*          status)
      SRX_NOEXCEPT override;

private:
    String MakeSystemPath(StringView path) const;

private:
    THashMap<hash_t, Catalog> mCatalogs;
  };
}
