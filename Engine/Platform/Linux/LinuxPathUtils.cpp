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

#include <unistd.h>

namespace Sorex::FileSystem
{
  SRX_API Path GetUserAppsDataPath() SRX_NOEXCEPT
  {
    if (const char* path = getenv("XDG_CONFIG_HOME"))
      return FileSystem::Path(path);

    FileSystem::Path path = getenv("HOME");
    SRX_ASSERT(!path.empty());

    return path / ".config";
  }

  SRX_API Path GetAppPath() SRX_NOEXCEPT
  {
    char    fullpath[PATH_MAX] = { 0 };
    ssize_t length             = readlink("/proc/self/exe", fullpath, PATH_MAX);

    SRX_ASSERT_MSG(length > 0 && length < PATH_MAX, "readlink() failed");

    fullpath[length] = '\0';
    return FileSystem::Path(fullpath).remove_filename();
  }
}  // namespace
