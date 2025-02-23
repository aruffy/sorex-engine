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
  class AssetManager final: public Director::Component
  {
    SRX_RTTI(Resource::AssetManager, Director::Component)

public:
    explicit AssetManager(AssetStorage&                 storage,
                          TUniquePointer<AssetRegistry> registry = nullptr);

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
     * @return TRef<T> - pointer to the asset
     */
    template<typename T>
    TRef<T> Load(StringView            name,
                 IAssetLoadingHandler* handler = nullptr,
                 const AssetOptions*   options = nullptr)
    {
      return LoadAsset<T>(ELoadingMode::Sync, name, handler, options);
    }

    /**
     * @brief Load asset asynchronously. Push asset loading task to the resource
     * thread.
     *
     * @tparam T - asset type
     * @param name - asset name (path) to seek in the storage
     * @param handler - asset handler that handle loding stages
     * @param options - options to pass into loader
     * @return TRef<T> - pointer to the asset
     */
    template<typename T>
      requires std::is_base_of_v<Asset, T>
    TSharedPointer<T> LoadAsync(StringView            name,
                                IAssetLoadingHandler* handler = nullptr,
                                const AssetOptions*   options = nullptr)
    {
      return LoadAsset<T>(ELoadingMode::Async, name, handler, options);
    }

    /**
     * @brief Lazy loading of an asset.
     *
     * @tparam T - asset type
     * @param name - asset name (path) to seek in the storage
     * @param handler - asset handler that handle loding stages
     * @param options - options to pass into loader
     * @return TRef<T> - pointer to the asset
     */
    template<typename T>
      requires std::is_base_of_v<Asset, T>
    TRef<T> LoadLazy(StringView            name,
                     IAssetLoadingHandler* handler = nullptr,
                     const AssetOptions*   options = nullptr)
    {
      return LoadAsset<T>(ELoadingMode::Deferred, name, handler, options);
    }

    /**
     * @brief Set loader for the certain asset type.
     */
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
                                    StringView            name,
                                    IAssetLoadingHandler* handler,
                                    const AssetOptions*   options);
    TSharedPointer<Asset> LoadAsset(const RuntimeClass&   type,
                                    StringView            name,
                                    ELoadingMode          mode,
                                    IAssetLoadingHandler* handler,
                                    const AssetOptions*   options);

    TUniquePointer<AssetLoader> CreateAssetLoader(const RuntimeClass& type,
                                                  StringView          name,
                                                  const AssetOptions* options,
                                                  Status* status) const;

private:
    mutable std::shared_mutex mMutex;

    AssetStorage&                 mAssetStorage;
    TUniquePointer<AssetRegistry> mAssetRegistry;

    // TUniquePointer<Worker>                                     _worker;
    THashMap<const RuntimeClass*, TUniquePointer<AssetCreator>> mAssetFactory;
  };

  template<typename T>
    requires std::is_base_of_v<Asset, T>
  TSharedPointer<T> AssetManager::LoadAsset(ELoadingMode          mode,
                                            StringView            name,
                                            IAssetLoadingHandler* handler,
                                            const AssetOptions*   options)
  {
    TSharedPointer<T> asset =
      mAssetRegistry ? mAssetRegistry->Get<T>(name) : nullptr;

    return asset
             ? asset
             : std::static_pointer_cast<T>(
                 LoadAsset(GetRuntimeClass<T>(), name, mode, handler, options));
  }

  template<typename T>
    requires std::is_base_of_v<Asset, T>
  SRX_INLINE void AssetManager::Register(TUniquePointer<AssetCreator> loader)
  {
    Register(GetRuntimeClass<T>(), std::move(loader));
  }
}  // namespace
