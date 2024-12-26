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
#include <Sorex/FileSystem/SxPathUtils.h>
#include <Sorex/FileSystem/SxFile.h>
#include <Sorex/Utils/SxString.h>

namespace
{
  static constexpr int kMaxDirDepth = 32;

  using namespace Sorex;
  using EntryList = THashMap<FileSystem::PathString, TVector<PathString>>;

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

    const FileSystem::PathString subdir = path.native().substr(offset);
    TVector<PathString>&         files  = entries[std::move(subdir)];

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
        files.push_back(it->path().filename().native());
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
  Directory::Directory(PathStringView path,
                       IFileSystem*   parent /* = nullptr */) SRX_NOEXCEPT
    : mBasepath(Utils::MakePathWithClosingSlash(path))
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

  StaticDirectory::StaticDirectory(PathStringView path,
                                   IFileSystem*   parent /* = nullptr */)
    : Directory(path, parent)
  {}

  Status StaticDirectory::Mount(const Path& path) SRX_NOEXCEPT
  {
    Path   dirSysPath;
    size_t offset = 0;
    if (auto parent = GetParent())
    {
      dirSysPath = parent->GetSystemPath();
      offset     = dirSysPath.native().length();
      dirSysPath = dirSysPath / path.relative_path();
    }
    else
    {
      dirSysPath = path;
    }

    SRX_DEBUG("[StaticDirectory] Mount path '{}'", dirSysPath.native());

    Status     status;
    EntryList  entries;
    const auto fileNumber =
      ::CollectFiles(dirSysPath, offset, entries, 0, status);

    if (!status.Ok() || fileNumber == 0)
      return status;

    SRX_DEBUG("[StaticDirectory] Dir '{}' indexed {} files",
              path.native(),
              fileNumber);

    mCatalogs.reserve(mCatalogs.size() + entries.size());
    for (auto& [dir, files] : entries)
    {
      if (dir.empty() || files.empty())
        continue;

      PathString dirname =
        Path(std::move(dir)).generic_string<Path::value_type>();

      if (dirname.back() == Utils::GetPathDelimiter())
        dirname.pop_back();

      const hash_t dirHash = GetHash(PathStringView(dirname));
      auto res = mCatalogs.emplace(dirHash, Catalog{ std::move(dirname), {} });

      SRX_CHECK(res.second);

      TVector<FileIndex>& indecies = res.first->second.files;
      indecies.resize(files.size());
      for (size_t i = 0; i < files.size(); ++i)
      {
        PathString& fileName  = files[i];
        FileIndex&  fileIndex = indecies[i];

        const hash_t fileHash = GetHash(PathStringView(fileName));

        // Store dir hash to fast search dir
        fileIndex.id         = dirHash;
        fileIndex.descriptor = fileHash ^ dirHash;
        fileIndex.filepath   = std::move(fileName);
      }

      indecies.shrink_to_fit();
    }

    return status;
  }

  Status StaticDirectory::IndexFiles() SRX_NOEXCEPT
  {
    return Mount(GetSystemPath());
  }

  void StaticDirectory::GetFiles(PathStringView      path,
                                 TVector<FileIndex>& files) SRX_NOEXCEPT
  {
    if (path.empty())
      return;

    const auto [dirname, filename] = Utils::SplitPath(path);
    auto it                        = mCatalogs.find(GetHash(dirname));

    if (it == mCatalogs.end())
      return;

    if (Utils::GetFileExtension(filename).empty() == false)
    {
      const auto [_, fileIndex] = GetFile(path);
      if (fileIndex.has_value())
        files.push_back(
          { fileIndex->id, fileIndex->descriptor, std::monostate() });

      return;
    }

    const bool bCollectAll = filename.empty();
    for (const FileIndex& fileIndex : it->second.files)
    {
      bool bPush = true;
      if (bCollectAll == false)
      {
        if (const PathString* filePath =
              std::get_if<PathString>(&fileIndex.filepath))
        {
          StringView ext = Utils::GetFileExtension(*filePath, true);
          StringView fname(filePath->data(), filePath->length() - ext.length());

          bPush = (filename == fname);
        }
      }

      if (bPush)
        files.push_back(
          FileIndex{ fileIndex.id, fileIndex.descriptor, std::monostate() });
    }
  }

  TPair<EFileStatus, TOptional<FileIndex>> StaticDirectory::GetFile(
    PathStringView path) const SRX_NOEXCEPT
  {
    const auto [dirname, filename] = Utils::SplitPath(path);
    if (dirname.empty() || filename.empty())
      return std::make_pair(EFileStatus::Unknown, std::nullopt);

    const hash_t dirHash = GetHash(dirname);
    auto         it      = mCatalogs.find(dirHash);

    if (it == mCatalogs.end())
      return std::make_pair(EFileStatus::Unknown, std::nullopt);

    const hash_t key = GetHash(filename) ^ dirHash;

    auto fileIt =
      std::find_if(it->second.files.begin(),
                   it->second.files.end(),
                   [key](const FileIndex& index) {
                     return std::get<hash_t>(index.descriptor) == key;
                   });

    return fileIt != it->second.files.end()
             ? std::make_pair<EFileStatus, TOptional<FileIndex>>(
                 EFileStatus::Existent,
                 TOptional<FileIndex>(
                   { fileIt->id, fileIt->descriptor, std::monostate() }))
             : std::make_pair(EFileStatus::Unknown, std::nullopt);
  }

  TUniquePointer<Stream> StaticDirectory::OpenFile(const FileIndex& fileIndex,
                                                   Status* status) SRX_NOEXCEPT
  {
    SRX_CHECK(std::holds_alternative<hash_t>(fileIndex.descriptor));

    auto dirIt = mCatalogs.find(fileIndex.id);
    if (dirIt != mCatalogs.end())
    {
      auto fileIt =
        std::find_if(dirIt->second.files.cbegin(),
                     dirIt->second.files.cend(),
                     [&fileIndex](const FileIndex& fidx) {
                       return fileIndex.descriptor == fidx.descriptor;
                     });

      if (fileIt != dirIt->second.files.cend())
      {
        const Path syspath =
          GetSystemPath() / std::get<PathString>(fileIt->filepath);

        return File::Open(syspath.generic_string(),
                          EAccessMode::Read,
                          File::EOpenMode::Binary,
                          status);
      }
    }

    if (status)
      *status = SRX_STATUS_MSG(EStatusCode::Not_Found,
                               "file not found: {}",
                               std::get<hash_t>(fileIndex.descriptor));

    return nullptr;
  }
}  // namespace
