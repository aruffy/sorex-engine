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

#include "SxPathUtils.h"

using namespace Sorex::FileSystem;

namespace
{
  typedef Path::value_type Char;
  constexpr Char           kSlash = Sorex::Utils::GetPathDelimiter<Char>();
}

namespace Sorex::Utils
{
  PathString CombinePath(const TVector<PathStringView>& dirs) SRX_NOEXCEPT
  {
    PathString   result;
    const size_t size = dirs.size();

    if (size == 0)
      return result;

    result = dirs[0];
    if (!result.empty() && result.back() == kSlash)
      result.pop_back();

    if (size == 1)
      return result;

    for (size_t i = 1; i < size; ++i)
    {
      PathStringView str = Utils::Trim(dirs[i], kSlash);
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

  String CombinePath(const TVector<String>& dirs) SRX_NOEXCEPT
  {
    TVector<StringView> views;
    views.resize(dirs.size());

    std::transform(dirs.begin(),
                   dirs.end(),
                   views.begin(),
                   [](const String& s) { return StringView(s); });

    return CombinePath(views);
  }

  String EnsureClosingSlash(StringView path) SRX_NOEXCEPT
  {
    if (path.empty())
      return String();

    if (path.back() == kSlash)
      return String(path);

    String res;
    res.reserve(path.length() + 1);
    res.assign(path);
    res.push_back(kSlash);

    return res;
  }

  StringView GetFileExtension(StringView path,
                              bool       bIncludeDot /* = false */) SRX_NOEXCEPT
  {
    constexpr StringView::value_type kDot = '.';

    if (path.empty() || path.back() == kDot)
      return StringView();

    const size_t indx = path.rfind(kDot);
    if (indx == path.npos)
      return StringView();

    return bIncludeDot ? path.substr(indx) : path.substr(indx + 1);
  }

  TPair<StringView, StringView> SplitPath(StringView path,
                                          bool bClosingSlash /* = false */)
    SRX_NOEXCEPT
  {
    const size_t length = path.length();
    if (length == 0)
      return std::make_pair<StringView, StringView>({}, {});

    if (length == 1 && path[0] == kSlash)
      return std::make_pair((bClosingSlash ? StringView(path) : StringView()),
                            StringView());

    const size_t indx = path.rfind(kSlash);
    if (indx == path.npos)
      return std::make_pair(path, StringView());

    return std::make_pair(
      path.substr(0, (bClosingSlash ? (indx + 1) : indx)),
      ((indx == length - 1) ? StringView() : path.substr(indx + 1)));
  }

  void SplitPath(StringView             path,
                 TPair<String, String>& out,
                 bool bClosingSlash /* = false */) SRX_NOEXCEPT
  {
    auto p = SplitPath(path, bClosingSlash);
    out.first.assign(p.first);
    out.second.assign(p.second);
  }

  StringView GetBaseName(StringView path,
                         bool       bClosingSlash /* = false */) SRX_NOEXCEPT
  {
    constexpr StringView kEmptyStringView;

    const size_t length = path.length();
    if (length == 0)
      return kEmptyStringView;

    if (path.back() == kSlash)
      return StringView(path.data(), (bClosingSlash ? length : (length - 1)));

    const size_t pos = path.find_last_of(kSlash);
    if (pos != StringView::npos)
      return path.substr(0, (bClosingSlash ? (pos + 1) : pos));

    return kEmptyStringView;
  }

  StringView GetRootName(StringView path,
                         bool       bClosingSlash /* = false */) SRX_NOEXCEPT
  {
    const size_t length = path.length();
    if (length == 0)
      return StringView();

    if (length == 1 && path[0] == kSlash)
      return bClosingSlash ? path : StringView();

    const size_t indx = path.find(kSlash, 1);
    if (indx == path.npos)
      return (path[0] == kSlash && bClosingSlash) ? StringView(path.data(), 1)
                                                  : StringView();

    return path.substr(0, (bClosingSlash ? (indx + 1) : indx));
  }
}
