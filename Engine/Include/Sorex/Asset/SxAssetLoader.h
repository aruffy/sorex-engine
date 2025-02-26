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
#include <Sorex/Utils/SxString.h>

#include "SxAsset.h"
#include "SxAssetStorage.h"
#include "SxAssetRegistry.h"
#include "SxAssetDependence.h"
#include "SxAssetOptions.h"

namespace Sorex::Resource
{
  enum class ELoadingMode
  {
    Sync,   ///< Synchronous (block) loading
    Async,  ///< Async laoding in resouce thread pool
  };

  class AssetLoader
  {
public:
    explicit AssetLoader(TSharedPointer<Asset> asset) SRX_NOEXCEPT
      : mAsset(std::move(asset))
    {
      SRX_ASSERT(mAsset);
    }

    virtual ~AssetLoader() = default;

    AssetLoader(const AssetLoader&)            = delete;
    AssetLoader& operator=(const AssetLoader&) = delete;

    TSharedPointer<Asset> GetAsset() const { return mAsset; }
    Asset*                GetAssetPtr() { return mAsset.get(); }
    const Asset*          GetAssetPtr() const { return mAsset.get(); }

    const String& GetAssetName() const
    {
      return mAsset ? mAsset->GetName() : Utils::kEmptyString;
    }

    /**
     * @brief Do preparation before loading resource.
     *
     * If missing files container isn'st empty probably needed files are being
     * downloaded. The main idea of this function to sure that storage ready for
     * resource loading.
     *
     * @note can be call only from any thread.
     *
     * @param storage - resource storage
     * @param missingFiles - containts files that needed to load resorurce.
     * @return preload operation status
     */
    virtual Status Preload(AssetStorage&    storage,
                           AssetRegistry*   registry,
                           TVector<String>& missingFiles) = 0;

    /**
     * @brief Start loading of the asset.
     *
     * This function can be invoked from either app or background thread.
     * So, Operation that must be execute only in the main application thread
     * shouldn't be here.
     *
     * @param storage - resources storage
     * @param dependencies - list of dependencies that should be loaded before
     * the asset;
     * @return loading operation status.
     */
    virtual Status Load(AssetStorage&       storage,
                        const AssetOptions* options,
                        AssetDependencies&  dependencies) = 0;

    /**
     * @brief Invoked from main application thread to end loading process.
     * Will be invoked after all dependencies are loaded from the app thread.
     *
     * @param dependencies - @todo: lookup
     * @param error - description of error
     * @return
     */
    virtual Status Finalize(AssetRegistry*           registry,
                            const AssetDependencies& dependencies) = 0;

private:
    TSharedPointer<Asset> mAsset;
  };
}  // namespace
