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

#include <Sorex/Asset/SxAssetLoader.h>

#include <Sorex/Utils/SxString.h>
#include <Sorex/FileSystem/SxPathUtils.h>

namespace Sorex::Resource
{
  // TODO: What is the point?
  String AssetLoader::FindResource(AssetStorage& storage) const
  {
    const String& name      = GetAssetName();
    StringView    extansion = Utils::GetFileExtension(name);

    if (extansion.empty())
    {
      TVector<String> paths;
      storage.GetAll(name,
                     paths);  // @todo: it's not efficient (asset metadata?)

      if (paths.empty())
        return Utils::kEmptyString;

      if (paths.size() != 1)
      {
        SRX_WARN("[AssetLoader] Resource {} isn't unique ({})",
                 name,
                 paths.size());
      }

      return std::move(paths.front());
    }

    return storage.Contains(name) ? name : Utils::kEmptyString;
  }
}
