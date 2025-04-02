/**************************************************************************/
/*                         This file is part of:                          */
/*                                SOREX                                   */
/*                 Simple OpenGL Rendering Engine eXtended                */
/**************************************************************************/
/* Copyright (c) 2022-2024 Aleksandr Ershov (Ruffy).                      */
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
#include <Sorex/SxTask.h>

#include "SxAsset.h"
#include "SxAssetStorage.h"
#include "SxAssetRegistry.h"

namespace Sorex::Resource
{
  class IAssetAwaiter
  {
public:
    virtual ~IAssetAwaiter() = default;

    virtual ETaskAction GetAssetStatus(const Asset& asset, Status* status) = 0;
  };

  /**
   * @class LoadingHandler - handle asset loading proccess.
   *
   * @note If you have one loader object for several resources it should be
   * thread safe; If you have one loader instance for one loadable asset it
   * could be called from different threads, but not in the race condition.
   *
   */
  class IAssetLoadingHandler
  {
public:
    virtual ~IAssetLoadingHandler() = default;

    /**
     * @brief OnLoadingFailed is called when asset loading was failed.
     *
     * The asset can be null if a loading wasn't start.
     *
     * @param name - containes name of the asset that is couse of the error
     * (root/dependency asset);
     * @param asset - the asset pointer for what loading was initiated (root
     * asset or null);
     * @param reason - error description.
     */
    virtual void OnAssetLoadingFailed(PathView name,
                                      Asset*     asset,
                                      Status&    reason)
    {}

    /**
     * @brief Invoked when asset and all its dependencies were loaded
     * successfully.
     *
     * @param registry - resource registry;
     * @param asset - loaded asset.
     */
    virtual void OnAssetLoaded(AssetRegistry* registry, Asset* asset) {}

    /**
     * @brief Invoked when a loadable resource hasn't found needed file to load.
     *
     * Return pair with action:
     *  Cancel - loading inpossible,
     *  Continue - ready to continue loading,
     *  Await - need to postpone task and wait; Must provide the
     * ILoadingAwaiter in this case.
     *
     * @param storage - asset storage;
     * @param registy - asset registry;
     * @param asset - loadable asset;
     * @param files - missing files;
     * @return action that caller should follow;
     */
    virtual TPair<ETaskAction, TUniquePointer<IAssetAwaiter>>
    HandleMissingFiles(AssetStorage&      storage,
                       AssetRegistry*     registy,
                       const Asset*       asset,
                       const TSpan<Path>& files)
    {
      return std::make_pair(ETaskAction::Cancel, nullptr);
    }
  };
}  // namespace
