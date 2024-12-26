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
#include <Sorex/SxStream.h>

#include <filesystem>

namespace Sorex::FileSystem
{
  using Path           = std::filesystem::path;
  using PathString     = Path::string_type;
  using PathStringView = BasicStringView<PathString::value_type>;

  enum class EFileStatus
  {
    Unknown,  ///< Unknown file doesn't exists
    Missing,  ///< File is missing (exp. downloading)
    Existent  ///< File exists and ready to work
  };

  SRX_API hash_t            GetHash(PathStringView path) SRX_NOEXCEPT;
  SRX_API SRX_INLINE hash_t GetHash(const Path& path) SRX_NOEXCEPT
  {
    return GetHash(PathStringView(path.native()));
  }

  // Platform implementation

  /**
   * @brief Retrieve path to writable directory of the user.
   *
   * @return directory path in the generic format.
   */
  SRX_API Path GetUserAppsDataPath() SRX_NOEXCEPT;

  /**
   * @brief Retrieve path to folder where application was started.
   *
   * @return directory path in the generic format
   */
  SRX_API Path GetAppPath() SRX_NOEXCEPT;

  /**
   * @struct FileIndex
   * @brief Represents an index for a file in the file system to optimize
   * access.
   *
   * The main purpose of the FileIndex is to allow the file system
   * implementaion to work with files more efficiently. It provides a way to
   * access files faster than using a path string, storing
   * implementation-specific data relevant to the file system.
   *
   * @var FileIndex::id
   * Unique identifier for the file.
   *
   * @var FileIndex::descriptor
   * A variant type that can hold an integer or hash_t representing
   * the file's descriptor.
   *
   * @var FileIndex::filepath
   * A variant type that can hold various representations of the file's
   * path, including monostate.
   */
  struct FileIndex
  {
    size_t                id;
    TVariant<int, hash_t> descriptor;
    TVariant<std::monostate,
             Path,
             TRef<const Path>,
             PathString,
             TRef<const PathString>>
      filepath;
  };

  class IFileSystem
  {
public:
    virtual ~IFileSystem() = default;

    /**
     * @brief Adds the specified path to the file system, making it and its
     * contents visible for file system.
     *
     * @param path The path to mount.
     * @return SRX_OK if the path was successfully mounted.
     */
    virtual Status Mount(const Path& path) SRX_NOEXCEPT = 0;

    /**
     * @brief Indexes the files in the file system.
     *
     * This function prepares the file system by indexing files, ensuring they
     * are ready for use. It should be called after all paths are mounted and
     * before accessing file system files.
     *
     * @return Status indicating the result of the indexing operation.
     */
    virtual Status IndexFiles() SRX_NOEXCEPT = 0;

    /**
     * @brief Retrieve list of all files from filesystem by the path.
     *
     *  If path is a directory it should provide all files in the directory;
     *  @note: if path is a directory it shouldn't search it in subdirs - no
     * recursion; If path is a path to the file it should provide all files with
     * the same name:
     *      `/path/to/file` can return `/path/to/file.xml`,
     * `/path/to/file.png`,
     *
     * @param path - path to search in generic format;
     * @param out files - list to store files;
     */
    virtual void GetFiles(PathStringView      path,
                          TVector<FileIndex>& files) SRX_NOEXCEPT = 0;


    // @TODO: FindFiles (recursion)

    /**
     * @brief Retrieve file status and its index if the file is found.
     *
     * @param filename - path to search for the file.
     * @return pair of file status and file index if the file is found.
     */
    virtual TPair<EFileStatus, TOptional<FileIndex>> GetFile(
      PathStringView filename) const SRX_NOEXCEPT = 0;

    /**
     * @brief Retrive status of a file.
     *
     * @param filename - path of the file.
     * @return status of the file.
     */
    virtual EFileStatus GetFileStatus(PathStringView filename) const
      SRX_NOEXCEPT;

    /**
     * @brief Check if file exists.
     *
     * @param path - path to the file
     * @return True if file exists in the file system.
     */
    SRX_INLINE bool IsFileExists(PathStringView path) const SRX_NOEXCEPT
    {
      return GetFileStatus(path) == FileSystem::EFileStatus::Existent;
    }

    /**
     * @brief Retrieve absolute path to the file system in the native path
     * format.
     *
     * @return system path to the file system.
     */
    virtual Path GetSystemPath() const SRX_NOEXCEPT = 0;

    /**
     * @brief Opens a file at the specified path.
     *
     * This function attempts to open a file and returns a pointer to the opened
     * file. If the operation fails, it sets the status accordingly.
     *
     * @param path The path to the file to be opened.
     * @param status Pointer to a Status object that will be updated with the
     * result of the operation.
     *
     * @return A unique pointer to the opened file, or NULL if the operation
     * failed.
     */
    virtual TUniquePointer<Stream> OpenFile(const FileIndex& fileIndex,
                                            Status* status) SRX_NOEXCEPT = 0;

    /**
     * @brief Opens a file specified by the given filepath.
     *
     * This function attempts to open a file and returns a unique pointer to the
     * stream if successful. If the file does not exist, it updates the provided
     * status pointer with an error message.
     *
     * @param filepath The path of the file to be opened.
     * @param status A pointer to a Status object that will be updated with
     * error information if the file is not found.
     *
     * @return A unique pointer to the opened stream, or nullptr if the file
     * does not exist.
     */
    virtual TUniquePointer<Stream> OpenFile(PathStringView filepath,
                                            Status*        status) SRX_NOEXCEPT;
  };
}  // namespace

using SxFileSystem = Sorex::FileSystem::IFileSystem;
