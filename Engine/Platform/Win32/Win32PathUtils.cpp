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

#include <Sorex/FileSystem/SxFileSystem.h>

#include <Windows.h>

#include <mmsystem.h>
#pragma comment(lib, "winmm")

namespace
{
  using namespace Sorex::FileSystem;
  enum class ESysDirectory
  {
    Common_AppData = 0,
    Common_Desktop,
    Common_Pictures,
    Common_Startup,
    User_AppData,
    User_Desktop,
    User_Pictures,
    User_Startup
  };

  Path GetDirectoryPath(ESysDirectory dirId, bool create)
  {
    // load the SHGetFolderPath function.
    typedef HRESULT(WINAPI
                    * PFNSHGETFOLDERPATHA)(HWND, int, HANDLE, DWORD, LPTSTR);
    PFNSHGETFOLDERPATHA pSHGetFolderPath = nullptr;
    HMODULE             hMod             = nullptr;

    hMod = LoadLibraryA("shell32.dll");
    if (hMod != nullptr)
    {
      pSHGetFolderPath =
        (PFNSHGETFOLDERPATHA)GetProcAddress(hMod, "SHGetFolderPathA");
      if (pSHGetFolderPath == nullptr)
        FreeLibrary(hMod);
    }

    if (pSHGetFolderPath == nullptr)
    {
      hMod = LoadLibraryA("shfolder.dll");
      if (hMod != nullptr)
      {
        pSHGetFolderPath =
          (PFNSHGETFOLDERPATHA)GetProcAddress(hMod, "SHGetFolderPathA");
        if (pSHGetFolderPath == nullptr)
          FreeLibrary(hMod);
      }
    }

    if (pSHGetFolderPath == nullptr)
      return Sorex::String();


    int folderFlags = 0;

#define CSIDL_COMMON_APPDATA 0x0023  // All Users\Application Data
#define CSIDL_APPDATA \
  0x001a  // C:\Documents and Settings\username\Application Data
#define CSIDL_COMMON_DESKTOPDIRECTORY 0x0019  // All Users\Desktop
#define CSIDL_DESKTOPDIRECTORY 0x0010         // <user name>\Desktop
#define CSIDL_COMMON_PICTURES 0x0036          // All Users\My Pictures
#define CSIDL_MYPICTURES 0x0027               // C:\Program Files\My Pictures
#define CSIDL_COMMON_STARTUP 0x0018           // All Users\Startup
#define CSIDL_STARTUP 0x0007                  // Start Menu\Programs\Startup
#define CSIDL_FLAG_CREATE \
  0x8000  // combine with CSIDL_ value to force folder creation in
          // SHGetFolderPath()

    switch (dirId)
    {
    case ESysDirectory::Common_AppData:
      folderFlags = CSIDL_COMMON_APPDATA;
      break;
    case ESysDirectory::User_AppData:
      folderFlags = CSIDL_APPDATA;
      break;
    case ESysDirectory::Common_Desktop:
      folderFlags = CSIDL_COMMON_DESKTOPDIRECTORY;
      break;
    case ESysDirectory::User_Desktop:
      folderFlags = CSIDL_DESKTOPDIRECTORY;
      break;
    case ESysDirectory::Common_Pictures:
      folderFlags = CSIDL_COMMON_PICTURES;
      break;
    case ESysDirectory::User_Pictures:
      folderFlags = CSIDL_MYPICTURES;
      break;
    case ESysDirectory::Common_Startup:
      folderFlags = CSIDL_COMMON_STARTUP;
      break;
    case ESysDirectory::User_Startup:
      folderFlags = CSIDL_STARTUP;
      break;
    };

    if (create)
      folderFlags |= CSIDL_FLAG_CREATE;

    char dirPath[MAX_PATH];

    if (FAILED(pSHGetFolderPath(nullptr, folderFlags, nullptr, 0, dirPath)))
      return Sorex::String();

    FreeLibrary(hMod);

    return Path(dirPath).make_preferred();
  }
}

namespace Sorex::FileSystem
{
  Path GetUserAppsDataPath() SRX_NOEXCEPT
  {
    static Path _appDataDirectory;

    if (_appDataDirectory.empty())
    {
      _appDataDirectory = GetDirectoryPath(ESysDirectory::User_AppData, false);
      if (_appDataDirectory.empty())
        _appDataDirectory = GetAppPath();
    }

    SRX_ASSERT(_appDataDirectory.empty() == false);
    return _appDataDirectory;
  }

  Path GetAppPath() SRX_NOEXCEPT
  {
    CHAR path[MAX_PATH] = { 0 };
    ::GetModuleFileNameA(NULL, path, MAX_PATH);

    return std::filesystem::path(path).remove_filename();
  }
}  // namespace
