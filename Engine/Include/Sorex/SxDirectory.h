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

#include "CoreMinimal.h"
#include "SxFileSystem.h"

namespace Sorex::FileSystem
{
  class Directory: public IFileSystem
  {
public:
    explicit Directory(StringView   path,
                       IFileSystem* parent = nullptr) SRX_NOEXCEPT;
    virtual ~Directory() override {}

    virtual Path GetSystemPath() const SRX_NOEXCEPT override;

    const Path&  GetBasePath() const { return mBasepath; }
    IFileSystem* GetParent() const { return mParent; }

protected:
    using EntryList = THashMap<PathStr, TVector<String>>;

    struct FileIndex
    {
      size_t id   = 0;
      hash_t hash = 0;

      String   name;
      PathView directory;
    };

    struct Catalog
    {
      Catalog() = default;
      SRX_INLINE Catalog(const PathStr& ps,
                         String&&       str,
                         size_t         num) SRX_NOEXCEPT
        : name(std::move(str))
        , path(ps)
      {
        files.reserve(num);
      }

      String name;              ///< Catalog name: @TODO: Should be replaced by
                                ///< WString/UTF8String?
      PathStr            path;  ///< Related path in a platform preferred format
      TVector<FileIndex> files;
    };

protected:
    /**
     * @brief Recursively collect file from current directory. Max depth is
     * equal of kMaxDirDepth;
     *
     *  Check if error code is not the No_Error, that at least one error
     * occured.
     *
     * @param entries - store collected <dir, files> pairs;
     * @param error - error description;
     * @return total number of collected files.
     */
    int32 CollectFiles(EntryList& entries, Status& status) SRX_NOEXCEPT;

private:
    Path mBasepath;  ///< Contains related system path in a platform preferred
                     ///< format
    IFileSystem* mParent;
  };

  class StaticDirectory final: public Directory
  {
public:
    explicit StaticDirectory(StringView path, IFileSystem* parent = nullptr);

    virtual Status IndexFiles() SRX_NOEXCEPT override;

    virtual void        GetFiles(StringView       path,
                                 TVector<String>& files) SRX_NOEXCEPT override;
    virtual EFileStatus GetFileStatus(StringView path) SRX_NOEXCEPT override;

    virtual TUniquePointer<Stream> OpenFile(StringView path, Status* status)
      SRX_NOEXCEPT override;

private:
    String MakeSystemPath(StringView path) const;

private:
    THashMap<hash_t, Catalog> mDirs;
  };
}
