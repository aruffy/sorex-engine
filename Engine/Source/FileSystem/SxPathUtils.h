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

using namespace Sorex::FileSystem;

namespace Sorex::Utils
{
  template<typename Char = char>
  SRX_INLINE constexpr Char GetPathDelimiter() SRX_NOEXCEPT
  {
    if constexpr (std::is_same_v<Char, char>)
      return Char('/');

    return static_cast<Char>(L'/');
  }

  /**
   * @brief Combine dirs to a path.
   *
   * @return combined path in generic format
   */
  PathString CombinePath(const TVector<PathStringView>& dirs) SRX_NOEXCEPT;
  String     CombinePath(const TVector<String>& dirs) SRX_NOEXCEPT;

  /**
   * @brief Ensure that path closed with slash.
   *
   * @return path with slash on the end
   */
  String EnsureClosingSlash(StringView path) SRX_NOEXCEPT;

  /**
   * @brief Retrieve extension of file.
   *
   * @param path - path to the file;
   * @param bIncludeDot - include dot of the extension in result;
   * @return extension of the file by path;
   */
  StringView GetFileExtension(StringView path,
                              bool       bIncludeDot = false) SRX_NOEXCEPT;

  /**
   * @brief Split path to two parts <dirname, filename>.
   *
   *
   * @param path - path for decomposing
   * @param bClosingSlash - enable closing slash for `dirname`
   * @return pair <dirname, filename>.
   */
  TPair<StringView, StringView> SplitPath(StringView path,
                                          bool       bClosingSlash = false)
    SRX_NOEXCEPT;

  /**
   * @brief Split path to two parts <dirname, filename>.
   *
   * @param path - path for decomposing
   * @param bClosingSlash - enable closing slash for `dirname`
   * @param out - pair of string to store <dirname, filename>
   */
  void SplitPath(StringView             path,
                 TPair<String, String>& out,
                 bool                   bClosingSlash = false) SRX_NOEXCEPT;

  /**
   * @brief Return dir (base) path.
   *
   * It work only with generic format (with '/' slach separator)
   * Return directory name in format `/path/to/dir`.
   * If bClosingSlash is True the closing slash will be included
   * `/path/to/dir/`.
   *
   * @note: don't use with string literal
   *
   * @param path - path for decomposing
   * @param bClosingSlash - enable closing slash
   * @return directory name or empty string if path invalid.
   */
  StringView GetBaseName(StringView path,
                         bool       bClosingSlash = false) SRX_NOEXCEPT;

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
   * @param base - string that will be store the result
   */
  void GetBaseName(StringView path,
                   String&    base,
                   bool       bClosingSlash = false) SRX_NOEXCEPT;

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
  StringView GetRootName(StringView path,
                         bool       bClosingSlash = false) SRX_NOEXCEPT;

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
   * @param root - string to store name of root directory or empty string if
   * path invalid.
   */
  SRX_INLINE void GetRootName(StringView path,
                              String&    root,
                              bool       bClosingSlash = false) SRX_NOEXCEPT;

}

SRX_INLINE void Sorex::Utils::GetBaseName(StringView path,
                                          String&    basename,
                                          bool bClosingSlash /* = false */)
  SRX_NOEXCEPT
{
  basename = Sorex::Utils::GetBaseName(path, bClosingSlash);
}

SRX_INLINE void Sorex::Utils::GetRootName(StringView path,
                                          String&    root,
                                          bool bClosingSlash /* = false */)
  SRX_NOEXCEPT
{
  root = Sorex::Utils::GetRootName(path, bClosingSlash);
}
