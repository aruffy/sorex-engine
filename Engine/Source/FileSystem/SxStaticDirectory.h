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

namespace Sorex::FileSystem
{
  // @NOTE: This isn't effecient class to work with the file system.
  // @TODO: Optimize it according to the needs.
  // @TODO: Don't store names of directories and files.
  class StaticDirectory final: public Directory
  {
public:
    explicit StaticDirectory(Path path) SRX_NOEXCEPT;

    virtual Status IndexFiles() SRX_NOEXCEPT override;
    virtual void   GetFiles(const Path&         path,
                            TVector<FileIndex>& files) SRX_NOEXCEPT override;

    virtual TPair<EFileStatus, TOptional<FileIndex>> GetFileIndex(
      const Path& path) const SRX_NOEXCEPT override;

    virtual TUniquePointer<Stream> OpenFile(
      const FileIndex& fileIndex,
      EAccessMode      mode   = EAccessMode::Read,
      Status*          status = nullptr) SRX_NOEXCEPT override;

private:
    int32 CollectFiles(const Path& path,
                       int32       depth,
                       Status&     status) SRX_NOEXCEPT;

private:
    struct Catalog
    {
      Path               path;
      TVector<FileIndex> files;  // TODO: Use THashSet<FileIndex>
    };

    THashMap<hash_t, Catalog> mCatalogs;
  };
}  // namespace
