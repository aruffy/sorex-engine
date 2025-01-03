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

#include <Sorex/FileSystem/SxDirectorFileSystem.h>
#include <Sorex/FileSystem/SxDirectory.h>
#include <Sorex/FileSystem/SxPathUtils.h>
#include <Sorex/Utils/SxString.h>

#include <FileSystem/SxStaticDirectory.h>

namespace Sorex
{
  Status DirectorFileSystem::Initialize()
  {
    SRX_CLSFUN_TRACE();
    SRX_CHECK(!mInited);

    const Status status = IndexFiles();
    mInited             = status.Ok();

    return status;
  }

  void DirectorFileSystem::Shutdown()
  {
    SRX_CLSFUN_TRACE();
  }

  Status DirectorFileSystem::Mount(const Path&    path,
                                   PathStringView alias) SRX_NOEXCEPT
  {
    if (alias.empty() || alias.find(Path::preferred_separator, 1) != alias.npos)
      return SRX_STATUS_MSG(EStatusCode::Invalid_Argument,
                            "requires alias to mount without separator");

    MutexLock _lock(mMutex);

    const hash_t key = GetHash(alias);
    if (auto it = mFilesystems.find(key); it != mFilesystems.end())
      return SRX_STATUS_MSG(
        EStatusCode::Not_Unique,
        "file system '{}' has already been mounted for '{}'",
        Path(alias).generic_string(),
        it->second.path.generic_string());

    // @TODO: add other file systems

    Path dirPath = path.is_absolute() ? path : (GetSystemPath() / path);
    if (!std::filesystem::is_directory(dirPath))
      return SRX_STATUS_MSG(EStatusCode::Invalid_Argument,
                            "invalid path to mount: '{}'",
                            dirPath.generic_string());

    auto dirPtr = MakeUnique<StaticDirectory>(std::move(dirPath));
    if (mInited)
    {
      if (Status status = dirPtr->IndexFiles(); !status.Ok())
        return status;
    }

    FileSystemInstance& fs = mFilesystems[key];
    fs.filesystem          = std::move(dirPtr);
    fs.path                = path;
    fs.path.make_preferred();

    return SRX_OK;
  }

  Status DirectorFileSystem::IndexFiles() SRX_NOEXCEPT
  {
    Status    status;
    MutexLock _lock(mMutex);

    // cppcheck-suppress unusedVariable
    for (auto& [_, fs] : mFilesystems)
    {
      status = fs.filesystem->IndexFiles();

      if (!status.Ok())
        return status;
    }

    return status;
  }

  void DirectorFileSystem::GetFiles(const Path&         path,
                                    TVector<FileIndex>& files) SRX_NOEXCEPT
  {
    MutexLock _lock(mMutex);

    files.clear();

    if (auto [fs, fspath] = GetFileSystemWithPath(path); fs)
    {
      fs->GetFiles(fspath, files);
      for (FileIndex& fileIndex : files)
      {
        // @FIXME: how to find file system by a file index
        SRX_ASSERT(std::holds_alternative<std::monostate>(fileIndex.filepath));
        fileIndex.filepath = PathString(GetFileSystemName(path.native()));
      }
    }
  }

  TPair<EFileStatus, TOptional<FileIndex>> DirectorFileSystem::GetFileIndex(
    const Path& filepath) const SRX_NOEXCEPT
  {
    MutexLock _lock(mMutex);

    auto [fs, path] = GetFileSystemWithPath(filepath);
    if (fs)
    {
      auto res = fs->GetFileIndex(path);
      if (res.second.has_value())
      {
        // @FIXME: how to find file system by a file index
        SRX_ASSERT(
          std::holds_alternative<std::monostate>(res.second->filepath));
        res.second->filepath = PathString(GetFileSystemName(filepath.native()));
      }

      return res;
    }

    return std::make_pair(EFileStatus::Unknown, std::nullopt);
  }

  EFileStatus DirectorFileSystem::GetFileStatus(const Path& filename) const
    SRX_NOEXCEPT
  {
    MutexLock _lock(mMutex);

    if (auto [fs, path] = GetFileSystemWithPath(filename); fs)
      return fs->GetFileStatus(path);

    return EFileStatus::Unknown;
  }

  IFileSystem* DirectorFileSystem::GetFileSystem(PathStringView path)
    SRX_NOEXCEPT
  {
    auto it = mFilesystems.find(GetHash(GetFileSystemName(path)));
    return it != mFilesystems.end() ? it->second.filesystem.get() : nullptr;
  }

  const IFileSystem* DirectorFileSystem::GetFileSystem(
    PathStringView path) const SRX_NOEXCEPT
  {
    auto it = mFilesystems.find(GetHash(GetFileSystemName(path)));
    return it != mFilesystems.cend() ? it->second.filesystem.get() : nullptr;
  }

  TPair<IFileSystem*, Path> DirectorFileSystem::GetFileSystemWithPath(
    const Path& path) const SRX_NOEXCEPT
  {
    PathStringView fsname = GetFileSystemName(path.native());
    if (auto it = mFilesystems.find(GetHash(fsname)); it != mFilesystems.end())
    {
      const Path& fspath = it->second.path;
      SRX_CHECK(fsname.length() < path.native().length());
      const Path relativePath =
        Path(path.native().substr(fsname.length())).relative_path();

      Path res = fspath.is_absolute()
                   ? (fspath / relativePath.relative_path())
                   : (GetSystemPath() / fspath / relativePath.relative_path());

      return std::make_pair<IFileSystem*, Path>(it->second.filesystem.get(),
                                                std::move(res));
    }

    return std::make_pair<IFileSystem*, Path>(nullptr, {});
  }

  PathStringView DirectorFileSystem::GetFileSystemName(PathStringView path)
    SRX_NOEXCEPT
  {
    auto root = Utils::GetRootName<SRX_PATH('/')>(path);
    return root.empty() ? path : root;
  }

  const Path& DirectorFileSystem::GetSystemPath() const SRX_NOEXCEPT
  {
    static const Path appPath = FileSystem::GetAppPath();
    return appPath;
  }

  TUniquePointer<Stream> DirectorFileSystem::OpenFile(
    const FileIndex& fileIndex,
    EAccessMode      mode,
    Status*          status) SRX_NOEXCEPT
  {
    if (const PathString* fsName = std::get_if<PathString>(&fileIndex.filepath))
    {
      SRX_DEBUG("[DirectorFileSystem] OpenFile fs:'{}' id:{}",
                Path(*fsName).generic_string(),
                fileIndex.id);

      MutexLock _lock(mMutex);

      if (IFileSystem* fs = GetFileSystem(*fsName))
        return fs->OpenFile(fileIndex, mode, status);
    }

    if (status)
      *status =
        SRX_STATUS_MSG(EStatusCode::Not_Found, "file system isn't found");

    return nullptr;
  }

  /*
    const String& DirectorFileSystem::GetAppDataPath() SRX_NOEXCEPT
    {
      return Utils::kEmptyString;

      // @TODO:

       const Application* app = GetApp();
   if (app == nullptr)
   {
     RFY_NOENTRY("Invalid application");
     return _appDataPath;
   }

   if (_appDataPath.empty())
   {
     const String syspath = PathUtils::GetUserAppsDataPath().generic_string();
     const Application::Info& appInfo = app->GetInfo();

     _appDataPath = PathUtils::Combine(
       TVector<StringView>{ syspath, appInfo.vendor, appInfo.name });
   }

   return _appDataPath;
} */
}
