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

namespace Sorex::FileSystem
{
  SRX_API static SRX_INLINE hash_t GetHash(PathStringView path) SRX_NOEXCEPT
  {
    static const THash<PathStringView> kPathViewHasher;
    return kPathViewHasher(path);
  }

  EFileStatus IFileSystem::GetFileStatus(PathStringView filename) const
    SRX_NOEXCEPT
  {
    return GetFile(filename).first;
  }

  TUniquePointer<Stream> IFileSystem::OpenFile(PathStringView filepath,
                                               Status* status) SRX_NOEXCEPT
  {
    const auto [fileStatus, fileIndex] = GetFile(filepath);
    if (fileStatus != EFileStatus::Existent || !fileIndex.has_value())
    {
      if (status)
        *status = SRX_STATUS_MSG(EStatusCode::Not_Found,
                                 "file '{}' not found",
                                 filepath);
      return nullptr;
    }

    return OpenFile(fileIndex.value(), status);
  }
};

}  // namespace
