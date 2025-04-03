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
#include <Sorex/SxDirector.h>

#include "SxFileSystem.h"

namespace Sorex
{
  using namespace Sorex::FileSystem;

  /**
   * @class DirectorFileSystem
   * @brief Represents the root file system of the director (application).
   *
   * This class simplifies interactions with the file system,
   * collecting resources for the application and providing easy access
   * to them using a generic path format with '/' as the separator.
   *
   * Example: filesystem->Mount("/MyData/Folder");
   * filesystem->GetFileIndex("/MyData/Folder/To/File.txt");
   */
  class DirectorFileSystem final
    : public Director::Component
    , public IFileSystem
  {
    SRX_RTTI(DirectorFileSystem, Director::Component);

public:
    // Director::Component Interface
    virtual Status Initialize() override;
    virtual void   Shutdown() override;

    // IFileSystem Interface
    Status         Mount(const Path& path,
                         PathView    alias = PathView()) SRX_NOEXCEPT override;
    virtual Status IndexFiles() SRX_NOEXCEPT override;

    virtual void GetFiles(const Path&         path,
                          TVector<FileIndex>& files) SRX_NOEXCEPT override;

    virtual TPair<EFileStatus, TOptional<FileIndex>> GetFileIndex(
      const Path& filepath) const SRX_NOEXCEPT override;
    virtual EFileStatus GetFileStatus(const Path& filename) const
      SRX_NOEXCEPT override;

    virtual const Path& GetSystemPath() const SRX_NOEXCEPT override;

    virtual TUniquePointer<Stream> OpenFile(const FileIndex& fileIndex,
                                            EAccessMode      mode,
                                            Status*          status)
      SRX_NOEXCEPT override;

    const IFileSystem* GetFileSystem(PathView path) const SRX_NOEXCEPT;

private:
    IFileSystem*    GetFileSystem(PathView path) SRX_NOEXCEPT;
    static PathView GetFileSystemName(PathView path) SRX_NOEXCEPT;

    TPair<IFileSystem*, Path> GetFileSystemWithPath(const Path& path) const
      SRX_NOEXCEPT;

private:
    bool mInited = false;

    struct FileSystemInstance
    {
      Path                        path;
      TUniquePointer<IFileSystem> filesystem;
    };

    mutable Mutex                        mMutex;
    THashMap<hash_t, FileSystemInstance> mFilesystems;
  };
}
