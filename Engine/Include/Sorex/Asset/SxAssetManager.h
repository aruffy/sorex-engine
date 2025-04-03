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
#include <Sorex/SxDirector.h>

#include "SxAsset.h"
#include "SxAssetCreator.h"
#include "SxAssetRegistry.h"
#include "SxAssetStorage.h"
#include "SxAssetHandler.h"
#include "SxAssetOptions.h"

namespace Sorex::Resource
{
  class TaskWorker;
  class AssetManager final: public Director::Component
  {
    SRX_RTTI(Resource::AssetManager, Director::Component)

public:
    explicit AssetManager(AssetStorage&                 storage,
                          TUniquePointer<AssetRegistry> registry = nullptr);
    virtual ~AssetManager() override;

    // Iterface Director::Component
    virtual Status Initialize() override;
    virtual void   Shutdown() override;
    virtual void   Update(const float deltaTime) override;

    /**
     * @brief Load asset in the main thread. After call this function asset
     * ready to use or invalid.
     *
     * @tparam T - asset type
     * @param name - asset name (path) to seek in the storage
     * @param handler - asset handler that handle loding stages
     * @param options - options to pass into loader
     * @return TSharedPointer<T> - pointer to the asset
     */
    template<typename T>
    TSharedPointer<T> Load(Path                  path,
                           IAssetLoadingHandler* handler = nullptr,
                           const AssetOptions*   options = nullptr)
    {
      return LoadAsset<T>(ELoadingMode::Sync,
                          std::move(path),
                          handler,
                          options);
    }

    /**
     * @brief Load asset asynchronously. Push asset loading task to the resource
     * thread.
     *
     * @tparam T - asset type
     * @param name - asset name (path) to seek in the storage
     * @param handler - asset handler that handle loding stages
     * @param options - options to pass into loader
     * @return TSharedPointer<T> - pointer to the asset
     */
    template<typename T>
      requires std::is_base_of_v<Asset, T>
    TSharedPointer<T> LoadAsync(Path                  path,
                                IAssetLoadingHandler* handler = nullptr,
                                const AssetOptions*   options = nullptr)
    {
      return LoadAsset<T>(ELoadingMode::Async,
                          std::move(path),
                          handler,
                          options);
    }

    template<typename T>
      requires std::is_base_of_v<Asset, T>
    SRX_INLINE void Register(TUniquePointer<AssetCreator> loader);

private:
    AssetCreator* FindAssetCreator(const RuntimeClass& type) const;
    void          Register(const RuntimeClass&          type,
                           TUniquePointer<AssetCreator> creator);

    template<typename T>
      requires std::is_base_of_v<Asset, T>
    TSharedPointer<T>     LoadAsset(ELoadingMode          mode,
                                    Path                  path,
                                    IAssetLoadingHandler* handler,
                                    const AssetOptions*   options);
    TSharedPointer<Asset> LoadAsset(const RuntimeClass&   type,
                                    Path                  path,
                                    ELoadingMode          mode,
                                    IAssetLoadingHandler* handler,
                                    const AssetOptions*   options);

    AssetCreator::AssetInstance CreateAssetInstance(const RuntimeClass& type,
                                                    Path                path,
                                                    Status* status) const;

private:
    mutable std::shared_mutex mMutex;

    AssetStorage&                 mAssetStorage;
    TUniquePointer<AssetRegistry> mAssetRegistry;

    THashMap<const RuntimeClass*, TUniquePointer<AssetCreator>> mAssetFactory;

    // @TODO: add resource thread pool
    TUniquePointer<TaskWorker> mWorker;
  };

  template<typename T>
    requires std::is_base_of_v<Asset, T>
  TSharedPointer<T> AssetManager::LoadAsset(ELoadingMode          mode,
                                            Path                  path,
                                            IAssetLoadingHandler* handler,
                                            const AssetOptions*   options)
  {
    TSharedPointer<T> asset =
      mAssetRegistry ? mAssetRegistry->Get<T>(path) : nullptr;

    return asset ? asset
                 : std::static_pointer_cast<T>(LoadAsset(GetRuntimeType<T>(),
                                                         std::move(path),
                                                         mode,
                                                         handler,
                                                         options));
  }

  template<typename T>
    requires std::is_base_of_v<Asset, T>
  SRX_INLINE void AssetManager::Register(TUniquePointer<AssetCreator> loader)
  {
    Register(GetRuntimeType<T>(), std::move(loader));
  }
}  // namespace

using SxAssetManager = Sorex::Resource::AssetManager;
