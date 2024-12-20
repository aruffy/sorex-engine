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

#include <Sorex/FileSystem/SxDirectory.h>
#include <Sorex/FileSystem/SxFile.h>

#include <Sorex/Utils/SxString.h>

#include "SxPathUtils.h"

namespace
{
  static constexpr int kMaxDirDepth = 32;

  using namespace Sorex;
  using EntryList = THashMap<FileSystem::PathStr, TVector<String>>;

  int32 CollectFiles(const FileSystem::Path& path,
                     const size_t            offset,
                     EntryList&              entries,
                     int32                   depth,
                     Status&                 status) SRX_NOEXCEPT
  {
    SRX_TRACE("[Directory] Collect '{}' depth={}",
              path.generic_string(),
              depth);

    if (depth > kMaxDirDepth)
    {
      status = SRX_STATUS_MSG(EStatusCode::Out_Of_Range,
                              "collecting depth limit is reached");
      return 0;
    }

    SRX_ASSERT(offset < path.native().size());

    const FileSystem::PathStr subdir = path.native().substr(offset);
    TVector<String>&          files  = entries[std::move(subdir)];

    using Iterator = std::filesystem::directory_iterator;

    std::error_code errc;
    auto            it = Iterator(path, errc);

    if (errc)
    {
      status = Status(std::move(errc));
      return 0;
    }

    int32 fnum = 0;
    for (; it != Iterator(); ++it)
    {
      if (it->is_directory())
      {
        fnum += CollectFiles(it->path(), offset, entries, depth + 1, status);
      }
      else if (it->is_regular_file())
      {
        files.push_back(it->path().filename().string());
        fnum++;
      }
      else
      {
        SRX_NOENTRY("[Directory] Skip unsupported entry");
      }
    }

    return fnum;
  }
}

namespace Sorex::FileSystem
{
  Directory::Directory(StringView   path,
                       IFileSystem* parent /* = nullptr */) SRX_NOEXCEPT
    : mBasepath(Utils::EnsureClosingSlash(path))
    , mParent(parent)
  {
    SRX_CHECK(!mBasepath.empty());
    mBasepath.make_preferred();
  }

  Path Directory::GetSystemPath() const SRX_NOEXCEPT
  {
    if (mParent == nullptr)
      return mBasepath;

    return mParent->GetSystemPath() / mBasepath.relative_path();
  }

  int32 Directory::CollectFiles(THashMap<PathStr, TVector<String>>& entries,
                                Status& status) SRX_NOEXCEPT
  {
    Path syspath = GetSystemPath();
    if (syspath.empty())
    {
      status = SRX_STATUS_MSG(EStatusCode::Invalid_State,
                              "Invalid system path of the directory");
      return 0;
    }

    return ::CollectFiles(syspath,
                          syspath.native().length()
                            - mBasepath.native().length(),
                          entries,
                          0,
                          status);
  }

  StaticDirectory::StaticDirectory(StringView   path,
                                   IFileSystem* parent /* = nullptr */)
    : Directory(path, parent)
  {}

  Status StaticDirectory::IndexFiles() SRX_NOEXCEPT
  {
    if (!mDirs.empty())
      return SRX_STATUS_MSG(EStatusCode::Invalid_State,
                            "static dir '{}' has already been indexed",
                            GetBasePath().generic_string());

    Status    status;
    EntryList dirs;
    {
      const int32 fnum = CollectFiles(dirs, status);
      if (!status.Ok() || fnum)
        return status;

      SRX_DEBUG("Static dir '{}' indexed {} files",
                GetBasePath().generic_string(),
                fnum);
    }

    size_t findex = 0;
    mDirs.reserve(dirs.size());
    for (auto& [dir, files] : dirs)
    {
      if (dir.empty() || files.empty())
        continue;

      String dirname = Path(dir).generic_string();
      if (dirname.back() == Utils::GetPathDelimiter())
        dirname.pop_back();

      const hash_t dir_hash = Utils::GetHash(dirname);
      auto         res =
        mDirs.emplace(dir_hash,
                      Catalog{ dir, std::move(dirname), files.size() });

      SRX_CHECK(res.second);
      TVector<FileIndex>& indecies = res.first->second.files;
      for (String& filename : files)
      {
        const hash_t filehash = Utils::GetHash(filename) ^ dir_hash;
        indecies.push_back(
          { findex++, filehash, std::move(filename), res.first->second.path });
      }

      indecies.shrink_to_fit();
    }

    return status;
  }

  void StaticDirectory::GetFiles(StringView       path,
                                 TVector<String>& files) SRX_NOEXCEPT
  {
    if (path.empty())
      return;

    const auto [dirname, filename] = Utils::SplitPath(path);
    auto it                        = mDirs.find(GetHash(dirname));

    if (it == mDirs.end())
      return;

    for (const FileIndex& file : it->second.files)
    {
      if (filename.empty() == false)
      {
        StringView ext = Utils::GetFileExtension(file.name, true);
        StringView fname(file.name.data(), file.name.length() - ext.length());

        if (filename != fname)  // @TODO: Compare hashes
          continue;
      }

      files.push_back(Utils::CombinePath(
        std::initializer_list<StringView>{ it->second.name, file.name }));
    }
  }

  EFileStatus StaticDirectory::GetFileStatus(StringView path) SRX_NOEXCEPT
  {
    const auto [dirname, filename] = Utils::SplitPath(path);
    if (dirname.empty() || filename.empty())
      return EFileStatus::Unknown;

    const hash_t dirHash = Utils::GetHash(dirname);
    auto         it      = mDirs.find(dirHash);

    if (it == mDirs.end())
      return EFileStatus::Unknown;

    const hash_t key = Utils::GetHash(filename) ^ dirHash;
    auto         fileIt =
      std::find_if(it->second.files.begin(),
                   it->second.files.end(),
                   [key](const FileIndex& index) { return index.hash == key; });

    return fileIt != it->second.files.end() ? EFileStatus::Existent
                                            : EFileStatus::Unknown;
  }

  TUniquePointer<Stream> StaticDirectory::OpenFile(StringView path,
                                                   Status* status) SRX_NOEXCEPT
  {
    return File::Open(MakeSystemPath(path),
                      EAccessMode::Read,
                      File::EOpenMode::Binary,
                      status);
  }

  String StaticDirectory::MakeSystemPath(StringView path) const
  {
    if (path.empty())
      return String();

    if (IFileSystem* parent = GetParent())
    {
      if (path[0] == Utils::GetPathDelimiter())
        path = path.substr(1);

      return (parent->GetSystemPath() / path).generic_string();
    }

    return String(path);
  }
}
