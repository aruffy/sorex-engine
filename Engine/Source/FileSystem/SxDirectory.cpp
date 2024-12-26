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

namespace Sorex::FileSystem
{
  Directory::Directory(Path path) SRX_NOEXCEPT
    : mSystemPath(std::move(path.make_preferred()))
  {
    SRX_CHECK(std::filesystem::is_directory(mSystemPath));
  }

  Path Directory::GetSystemPath() const SRX_NOEXCEPT
  {
    return mSystemPath;
  }

  Status Directory::Mount(const Path& path) SRX_NOEXCEPT
  {
    SRX_CHECK(path != mSystemPath);

    if (std::filesystem::is_directory(path) == false)
      return SRX_STATUS_MSG(EStatusCode::Not_Found,
                            "invalid path '{}' to directory",
                            path.native());

    SRX_DEBUG("[Directory] Mount path '{}'", path.native());
    SRX_CHECK(std::find(mMountedPaths.cbegin(), mMountedPaths.cend(), path)
              == mMountedPaths.cend());

    mMountedPaths.push_back(std::move(path));
    return SRX_OK;
  }
}  // namespace
