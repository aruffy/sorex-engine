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

#include <Sorex/FileSystem/SxRootFileSystem.h>

#include <Sorex/Utils/String.h>
#include <Sorex/FileSystem/SxDirectory.h>

#include "SxPathUtils.h"

namespace Sorex
{
  Status RootFileSystem::Initialize()
  {
    SRX_CLSFUN_TRACE();
    SRX_CHECK(!mInited);

    const Status status = IndexFiles();
    mInited             = status.Ok();

    return status;
  }

  void RootFileSystem::Shutdown()
  {
    SRX_CLSFUN_TRACE();
  }

  Status RootFileSystem::Mount(StringView path) SRX_NOEXCEPT
  {
    StringView fsname = GetFileSystemName(path);
    if (fsname.empty())
      return SRX_STATUS_MSG(EStatusCode::Invalid_Argument,
                            "Invalid path to mount: '{}'",
                            path);

    // @TODO: check if dir exist

    MutexLock _lock(mMutex);

    const hash_t key = Utils::GetHash(fsname);
    if (auto it = mFilesystems.find(key); it != mFilesystems.end())
      return SRX_STATUS_MSG(EStatusCode::Not_Unique,
                            "file system '{}' is already mounted",
                            fsname);

    // @TODO: add dynamic dirs

    auto dir = MakeUnique<StaticDirectory>(path, this);
    if (mInited)
    {
      if (Status status = dir->IndexFiles(); !status.Ok())
        return status;
    }

    mFilesystems[key] = std::move(dir);

    // for (auto listener : _listeners)
    // listener->OnFileSystemMounted(fs);

    return SRX_OK;
  }

  Status RootFileSystem::IndexFiles() SRX_NOEXCEPT
  {
    Status    status;
    MutexLock _lock(mMutex);
    for (auto& [_, fs] : mFilesystems)
    {
      status = fs->IndexFiles();
      if (!status.Ok())
        return status;
    }

    return status;
  }

  void RootFileSystem::GetFiles(StringView       path,
                                TVector<String>& files) SRX_NOEXCEPT
  {
    MutexLock _lock(mMutex);

    if (IFileSystem* fs = GetFileSystem(path))
      fs->GetFiles(path, files);
  }

  EFileStatus RootFileSystem::GetFileStatus(StringView path) SRX_NOEXCEPT
  {
    MutexLock _lock(mMutex);

    if (IFileSystem* fs = GetFileSystem(path))
      return fs->GetFileStatus(path);

    return EFileStatus::Unknown;
  }

  IFileSystem* RootFileSystem::GetFileSystem(StringView path) SRX_NOEXCEPT
  {
    StringView fsname = GetFileSystemName(path);
    if (fsname.empty())
      return nullptr;

    auto it = mFilesystems.find(Utils::GetHash(fsname));
    return it != mFilesystems.end() ? it->second.get() : nullptr;
  }

  const IFileSystem* RootFileSystem::GetFileSystem(StringView path) const
    SRX_NOEXCEPT
  {
    StringView fsname = GetFileSystemName(path);
    if (fsname.empty())
      return nullptr;

    auto it = mFilesystems.find(Utils::GetHash(fsname));
    return it != mFilesystems.end() ? it->second.get() : nullptr;
  }

  StringView RootFileSystem::GetFileSystemName(StringView path) SRX_NOEXCEPT
  {
    StringView root = Utils::GetRootName(path);
    return root.empty() ? path : root;
  }

  Path RootFileSystem::GetSystemPath() const SRX_NOEXCEPT
  {
    static const Path appPath = FileSystem::GetAppPath();
    return appPath;
  }

  TUniquePointer<Stream> RootFileSystem::OpenFile(StringView path,
                                                  Status* status) SRX_NOEXCEPT
  {
    {
      MutexLock lock(mMutex);
      if (IFileSystem* fs = GetFileSystem(path))
        return fs->OpenFile(path, status);
    }

    if (status)
      *status = SRX_STATUS_MSG(EStatusCode::Not_Found,
                               "file system isn't found for path: '{}'",
                               path);
    return nullptr;
  }

  /*
    const String& RootFileSystem::GetAppDataPath() SRX_NOEXCEPT
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
