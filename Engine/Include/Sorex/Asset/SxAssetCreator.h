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

// #include <Sorex/SxCoreMinimal.h>

// #include "SxAsset.h"
// #include "SxAssetLoader.h"
// #include "Dependence.h"
#include "SxAssetOptions.h"
#include "SxAssetRegistry.h"
#include "SxAssetStorage.h"

/**
namespace Ruffy::Resource
{
class AssetCreator: public Object
{
RFY_RTTI(Resource::AssetCreator, Object)

public:
virtual ~AssetCreator() override = default;

 * @brief Create asset object instance that will be used by loader.
 *
 * @param registry - asset registry;
 * @param name - name of the asset;
 * @param options - asset creation/loading options
 * @param error - error description;
 * @return Pointer to the asset or null if error occured.
virtual TRef<Asset> CreateAssetInstance(Registry*      registry,
                                        StringView     name,
                                        const Options* options,
                                        Error*         error) = 0;
 */

/**
 * @brief Create loader for the asset instance.
 *
 * @param asset - loadable asset;
 * @param options - asset creation/loading options
 * @param error - description of an error
 * @return Pointer to the load task or null if error occured.
virtual TUniquePointer<AssetLoader> CreateAssetLoader(
  const TRef<Asset>& asset,
  Error*             error) = 0;

protected:
template<typename T>
static bool IsLoadableReference(const TRef<Asset>& asset) noexcept;
};

template<typename T>
bool AssetCreator::IsLoadableReference(const TRef<Asset>& asset) noexcept
{
if (!asset)
  return false;

const EAssetState state = asset->GetState();
if (state != EAssetState::Unloaded)
  return false;

return asset->template IsA<T>();
}
}
 */
