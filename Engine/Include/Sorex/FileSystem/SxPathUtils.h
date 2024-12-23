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
#  define SRX_PATH_STRV(str) Sorex::FileSystem::PathStringView(L##str)
#else
#  define SRX_PATH(str) (str)
#  define SRX_PATH_STRV(str) Sorex::FileSystem::PathStringView(str)
#endif

using namespace Sorex::FileSystem;

namespace Sorex::Utils
{
  template<typename Char = Path::value_type>
  SRX_API SRX_INLINE constexpr Char GetPathDelimiter() SRX_NOEXCEPT
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
  SRX_API PathString CombinePath(const TVector<PathStringView>& dirs)
    SRX_NOEXCEPT;
  SRX_API PathString CombinePath(const TVector<PathString>& dirs) SRX_NOEXCEPT;

  /**
   * @brief Ensure that path closed with slash.
   *
   * @return path with slash on the end
   */
  SRX_API PathString MakePathWithClosingSlash(PathStringView path) SRX_NOEXCEPT;
  SRX_API void       EnsurePathClosingSlash(PathString& path) SRX_NOEXCEPT;

  /**
   * @brief Retrieve extension of file.
   *
   * @param path - path to the file;
   * @param bIncludeDot - include dot of the extension in result;
   * @return extension of the file by path;
   */
  SRX_API PathStringView
  GetFileExtension(PathStringView path, bool bIncludeDot = false) SRX_NOEXCEPT;

  /**
   * @brief Split path to two parts <dirname, filename>.
   *
   *
   * @param path - path for decomposing
   * @param bClosingSlash - enable closing slash for `dirname`
   * @return pair <dirname, filename>.
   */
  SRX_API TPair<PathStringView, PathStringView> SplitPath(
    PathStringView path,
    bool           bClosingSlash = false) SRX_NOEXCEPT;

  /**
   * @brief Split path to two parts <dirname, filename>.
   *
   * @param path - path for decomposing
   * @param bClosingSlash - enable closing slash for `dirname`
   * @param out - pair of string to store <dirname, filename>
   */
  SRX_API void SplitPath(PathStringView                 path,
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
  SRX_API PathStringView GetBaseName(PathStringView path,
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
   * @param base - string that will be store the result
   */
  SRX_API SRX_INLINE void GetBaseName(PathStringView path,
                                      PathString&    base,
                                      bool bClosingSlash = false) SRX_NOEXCEPT;

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
  SRX_API StringView GetRootName(StringView path,
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
  SRX_API SRX_INLINE void GetRootName(PathStringView path,
                                      PathString&    root,
                                      bool bClosingSlash = false) SRX_NOEXCEPT;

}

SRX_INLINE void Sorex::Utils::GetBaseName(StringView path,
                                          String&    basename,
                                          bool bClosingSlash /* = false */)
  SRX_NOEXCEPT
{
  basename = Sorex::Utils::GetBaseName(path, bClosingSlash);
}

SRX_INLINE void Sorex::Utils::GetRootName(PathStringView path,
                                          PathString&    root,
                                          bool bClosingSlash /* = false */)
  SRX_NOEXCEPT
{
  root = Sorex::Utils::GetRootName(path, bClosingSlash);
}
