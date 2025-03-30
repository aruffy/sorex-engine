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
#include <Sorex/FileSystem/SxFileSystem.h>

#ifdef SOREX_PLATFORM_WIN32
#  define SRX_PATH(str) (L##str)
#  define SRX_PATH_STRV(str) Sorex::PathView(L##str)
#else
#  define SRX_PATH(str) (str)
#  define SRX_PATH_STRV(str) Sorex::PathView(str)
#endif

using namespace Sorex::FileSystem;

namespace Sorex::Utils
{
  SRX_API PathString CombinePath(const TVector<PathView>& dirs) SRX_NOEXCEPT;
  SRX_API PathString CombinePath(const TVector<PathString>& dirs) SRX_NOEXCEPT;

  // @TODO: EnsureTrailingSlash
  SRX_API PathString MakePathWithClosingSlash(PathView path) SRX_NOEXCEPT;
  SRX_API void       EnsurePathClosingSlash(PathString& path) SRX_NOEXCEPT;

  /**
   * @brief Retrieve extension of file.
   *
   * @param path - path to the file;
   * @param bIncludeDot - include dot of the extension in result;
   * @return extension of the file by path;
   */
  SRX_API PathView GetFileExtension(PathView path,
                                    bool     bIncludeDot = false) SRX_NOEXCEPT;

  /**
   * @brief Split path to two parts <dirname, filename>.
   *
   *
   * @param path - path for decomposing
   * @param bClosingSlash - enable closing slash for `dirname`
   * @return pair <dirname, filename>.
   */
  SRX_API TPair<PathView, PathView> SplitPath(PathView path,
                                              bool     bClosingSlash = false)
    SRX_NOEXCEPT;

  /**
   * @brief Split path to two parts <dirname, filename>.
   *
   * @param path - path for decomposing
   * @param bClosingSlash - enable closing slash for `dirname`
   * @param out - pair of string to store <dirname, filename>
   */
  SRX_API void SplitPath(PathView                       path,
                         TPair<PathString, PathString>& out,
                         bool bClosingSlash = false) SRX_NOEXCEPT;

  /**
   * @brief Return dir (base) path.
   *
   * It work only with generic format (with '/' slach separator)
   * Return directory name in format `/path/to/dir`.
   * If bClosingSlash is True the closing slash will be included
   * `/path/to/dir/`.
   *
   * @param path - path for decomposing
   * @param bClosingSlash - enable closing slash
   * @return directory name or empty string if path invalid.
   */
  template<Path::value_type separator = Path::preferred_separator>
  SRX_API PathView GetBaseName(PathView path,
                               bool     bClosingSlash = false) SRX_NOEXCEPT
  {
    const size_t length = path.length();
    if (length == 0)
      return {};

    if (path.back() == separator)
      return PathView(path.data(), (bClosingSlash ? length : (length - 1)));

    const size_t pos = path.find_last_of(separator);
    if (pos != PathView::npos)
      return path.substr(0, (bClosingSlash ? (pos + 1) : pos));

    return {};
  }

  /**
   * @brief Return root dir (base) of a path.
   *
   * It work only with generic format (with '/' slach separator)
   * Return directory name in format `/dir`
   * Example: `/dir_name/path/to/file` -> `/dir_name`
   * If bClosingSlash is True the closing slash will be included.
   *
   * @param path - path for decomposing
   * @param bClosingSlash - enable closing slash
   * @return name of root directory or empty string if path invalid.
   */
  template<Path::value_type separator = Path::preferred_separator>
  SRX_API PathView GetRootName(PathView path,
                               bool     bClosingSlash = false) SRX_NOEXCEPT
  {
    const size_t length = path.length();
    if (length == 0)
      return {};

    if (length == 1 && path[0] == separator)
      return bClosingSlash ? path : PathView();

    const size_t indx = path.find(separator, 1);
    if (indx == path.npos)
      return (path[0] == separator && bClosingSlash) ? PathView(path.data(), 1)
                                                     : PathView();

    return path.substr(0, (bClosingSlash ? (indx + 1) : indx));
  }
}  // namespace
