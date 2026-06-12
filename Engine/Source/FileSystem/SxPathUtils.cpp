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

#include <Sorex/Utils/SxString.h>

#include <Sorex/FileSystem/SxPathUtils.h>

using namespace Sorex::FileSystem;

namespace
{
  typedef Sorex::Path::value_type Char;
  constexpr Char                  kSlash = Sorex::Path::preferred_separator;
}

namespace Sorex::Utils
{
  PathString CombinePath(const TVector<PathView>& dirs) SRX_NOEXCEPT
  {
    PathString   result;
    const size_t size = dirs.size();

    if (size == 0)
      return result;

    result = Utils::TrimRight(dirs[0], kSlash);
    for (size_t i = 1; i < size; ++i)
    {
      PathView str = Utils::Trim(dirs[i], kSlash);
      if (str.empty())
      {
        SRX_NOENTRY("Warning: unexpected dir name");
        continue;
      }

      result.push_back(kSlash);
      result.append(str);
    }

    return result;
  }

  PathString CombinePath(const TVector<PathString>& dirs) SRX_NOEXCEPT
  {
    TVector<PathView> views;
    views.resize(dirs.size());

    std::transform(dirs.begin(),
                   dirs.end(),
                   views.begin(),
                   [](const PathString& s) { return PathView(s); });

    return CombinePath(views);
  }

  PathString MakePathWithClosingSlash(PathView path) SRX_NOEXCEPT
  {
    if (path.back() == kSlash)
      return PathString(path);

    if (path.empty())
      return PathString(kSlash, 1);

    PathString res;
    res.reserve(path.length() + 1);
    res.assign(path);
    res.push_back(kSlash);

    return res;
  }

  void EnsurePathClosingSlash(PathString& path) SRX_NOEXCEPT
  {
    if (path.empty() || path.back() != kSlash)
      path.push_back(kSlash);
  }

  PathView GetFileExtension(PathView path,
                            bool     bIncludeDot /* = false */) SRX_NOEXCEPT
  {
    constexpr Char kDot = SRX_PATH('.');

    if (path.empty() || path.back() == kDot || path.back() == kSlash)
      return {};

    size_t indx = path.rfind(kSlash);
    if (indx != path.npos)
      path = path.substr(indx + 1);

    indx = path.rfind(kDot);
    if (indx == path.npos || indx == 0)
      return {};

    return bIncludeDot ? path.substr(indx) : path.substr(indx + 1);
  }

  TPair<PathView, PathView> SplitPath(PathView path,
                                      bool     bClosingSlash /* = false */)
    SRX_NOEXCEPT
  {
    const size_t length = path.length();
    if (length == 0)
      return {};

    if (path.back() == kSlash)
      return std::make_pair(
        (bClosingSlash ? PathView(path) : path.substr(0, length - 1)),
        PathView());

    const size_t indx = path.rfind(kSlash);
    if (indx == path.npos)
      return std::make_pair(path, PathView());

    return std::make_pair(path.substr(0, (bClosingSlash ? (indx + 1) : indx)),
                          path.substr(indx + 1));
  }
}  // namespace
