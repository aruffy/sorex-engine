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
#include <Sorex/Director.h>

#include "SxFileSystem.h"

namespace Sorex
{
  using namespace Sorex::FileSystem;
  /**
   * @class App::FileSystem - represent the file system of the application.
   *      Use genereic format of the path with the '/' slash separator.
   */
  class RootFileSystem final
    : public Director::Component
    , public IFileSystem
  {
    SRX_RTTI(RootFileSystem, Director::Component);

public:
    // IFileSystem Interface
    virtual Status      IndexFiles() SRX_NOEXCEPT override;
    virtual void        GetFiles(StringView       path,
                                 TVector<String>& files) SRX_NOEXCEPT override;
    virtual EFileStatus GetFileStatus(StringView name) SRX_NOEXCEPT override;

    virtual Path GetSystemPath() const SRX_NOEXCEPT override;

    virtual TUniquePointer<Stream> OpenFile(StringView path, Status* status)
      SRX_NOEXCEPT override;

    // Director::Component Interface
    virtual Status Initialize() override;
    virtual void   Shutdown() override;

    /**
     * @brief Add the current path to the app file system.
     *
     * @note The path must be subdir of the application folder with leading
     * slash. Example: "/Textures/Props"
     *
     * @param path - path to directory to mount.
     * @param[out] error - description of error;
     * @return true if directory was mounted.
     */
    Status Mount(StringView path) SRX_NOEXCEPT;
    // const String& GetAppDataPath() SRX_NOEXCEPT;

private:
    IFileSystem*       GetFileSystem(StringView path) SRX_NOEXCEPT;
    const IFileSystem* GetFileSystem(StringView path) const SRX_NOEXCEPT;
    static StringView  GetFileSystemName(StringView path) SRX_NOEXCEPT;

private:
    bool mInited = false;

    mutable Mutex                                 mMutex;
    THashMap<hash_t, TUniquePointer<IFileSystem>> mFilesystems;

    String mAppDataPath;
  };
}
