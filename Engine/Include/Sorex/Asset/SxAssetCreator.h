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

#include "SxAsset.h"
#include "SxAssetLoader.h"
#include "SxAssetRegistry.h"

namespace Sorex::Resource
{
  class AssetCreator
  {
public:
    using AssetInstance =
      TPair<TSharedPointer<Asset>, TUniquePointer<AssetLoader>>;
    virtual ~AssetCreator() = default;

    /**
     * @brief Create an asset instance pair consisting of an asset and its
     * loader.
     *
     * This function initializes an asset instance and its associated loader
     * based on the provided name and registry.
     *
     * @param name - the name of the asset to be created.
     * @param registry - the asset registry used for asset management.
     * @param status [out] - status variable that will hold the creation status
     * of the asset instance.
     * @return A pair containing the asset and its loader. If the asset is
     * nullptr, an error has occurred, and the loader will also be nullptr. If
     * the loader is nullptr, the asset is ready for use.
     */
    virtual AssetInstance CreateAssetInstance(Path           path,
                                              AssetRegistry* registry,
                                              Status*        status) = 0;

protected:
    template<typename T>
    static bool IsLoadableReference(const Asset* asset) SRX_NOEXCEPT;
  };

  template<typename T>
  bool AssetCreator::IsLoadableReference(const Asset* asset) SRX_NOEXCEPT
  {
    if (!asset)
      return false;

    const EAssetState state = asset->GetState();
    if (state != EAssetState::Unloaded)
      return false;

    return asset->template IsA<T>();
  }
}  // namespace
