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

#include "CoreMinimal.h"
#include "Stream.h"

#include <filesystem>

namespace Sorex
{
  // TODO: documentation: use /dir/to/file.txt format of directory. The
  // FileSystem should format and map it
  namespace FileSystem
  {
    using Path     = std::filesystem::path;
    using PathStr  = BasicString<Path::value_type>;
    using PathView = BasicStringView<Path::value_type>;
    enum class EFileStatus
    {
      Unknown,  ///< Unknown file doesn't exists
      Missing,  ///< File is missing (exp. downloading)
      Existent  ///< File exists and ready to work
    };

    SRX_API static SRX_INLINE hash_t GetHash(const Path& path) SRX_NOEXCEPT
    {
      return GetHash(PathView(path.native()));
    }

    SRX_API static SRX_INLINE hash_t GetHash(const PathView path) SRX_NOEXCEPT
    {
      static const THash<PathView> kPathViewHasher;
      return kPathViewHasher(path);
    }

  }  // namespace

  class IFileSystem
  {
public:
    virtual ~IFileSystem() = default;

    virtual Status IndexFiles() SRX_NOEXCEPT = 0;


    /**
     * @brief Retrieve list of all files from filesystem by the path.
     *
     *  If path is a directory it should provide all files in the directory;
     *  If path is a path to the file it should provide all files with the same
     * name:
     *      `/path/to/file` can return `/path/to/file.xml`, `/path/to/file.png`,
     * ...
     *
     * @param path - path to search;
     * @param out files - list to store files;
     */
    virtual void GetFiles(StringView       path,
                          TVector<String>& files) SRX_NOEXCEPT = 0;

    /**
     * @brief Retrive status of a file.
     *
     * @param filename - name of the file.
     * @return status of the file.
     */
    virtual FileSystem::EFileStatus GetFileStatus(StringView filename)
      SRX_NOEXCEPT = 0;

    /**
     * @brief Retrive the file index.
     *
     * @param filename - name of the file.
     * @return index of the file.
    virtual const FileSystem::FileIndex* GetFileIndex(StringView filename) const
    = 0;
     */

    /**
     * @brief Check if file exists.
     *
     * @param path - path to the file
     * @return True if file exists in the file system.
     */
    SRX_INLINE bool IsFileExists(StringView path) SRX_NOEXCEPT
    {
      return GetFileStatus(path) == FileSystem::EFileStatus::Existent;
    }

    /**
     * @brief Retrieve absolute path to the file system in the native path
     * format.
     *
     * @return path to the file system.
     */
    virtual FileSystem::Path GetSystemPath() const SRX_NOEXCEPT = 0;

    /**
     * @brief Open file.
     *
     * @param path
     * @param error
     * @return pointer to the opened file or NULL.
     */
    virtual TUniquePointer<Stream> OpenFile(StringView path,
                                            Status*    status) SRX_NOEXCEPT = 0;
  };
}
