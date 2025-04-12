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

#include <Sorex/Asset/SxAssetManager.h>

#include <SxTaskWorker.h>
#include <Asset/SxAssetLoadingTask.h>

namespace
{
  using namespace Sorex;
  Status LoadAssetSync(TUniquePointer<Task> task)
  {
    if (!task)
      return SRX_STATUS(EStatusCode::Invalid_Argument);

    Status            status;
    const ETaskAction action = task->Execute();

    if (action == ETaskAction::Await || action == ETaskAction::Cancel)
    {
      if (action == ETaskAction::Await)
        status =
          SRX_STATUS_MSG(EStatusCode::Not_Supported,
                         "asset sync loading mode doesn't support awaiting");
      else
        status = SRX_STATUS_MSG(EStatusCode::Canceled, "asset loading failed");

      task->Shutdown();
      return status;
    }

    // TODO: Call Finalize in any case to deallocate resource
    status = task->Finalize();
    if (!status.Ok())
      task->Shutdown();

    return status;
  }
}  // namespace

namespace Sorex::Resource
{
  AssetManager::AssetManager(AssetStorage&                 storage,
                             TUniquePointer<AssetRegistry> registry)
    : mAssetStorage(storage)
    , mAssetRegistry(std::move(registry))
  {}

  AssetManager::~AssetManager()
  {}

  Status AssetManager::Initialize()
  {
    SRX_CLSFUN_TRACE();

    mWorker = MakeUnique<TaskWorker>("AssetLoadingThread");
    return SRX_STATUS(mWorker->Start());
  }

  void AssetManager::Shutdown()
  {
    SRX_CLSFUN_TRACE();

    if (mWorker)
      mWorker->Stop();
  }

  void AssetManager::Update(const float deltaTime)
  {
    SRX_CHECK(mWorker);

    if (mWorker->HasCompletedTask())
    {
      if (auto task = mWorker->Pop())
      {
        // TODO: Call Finalize in any case to deallocate resource
        if (task->Finalize() != SRX_OK)
          task->Shutdown();
      }
    }

    if (mAssetRegistry)
      mAssetRegistry->Update(deltaTime);
  }

  TSharedPointer<Asset> AssetManager::LoadAsset(const RuntimeClass&   type,
                                                Path                  path,
                                                ELoadingMode          mode,
                                                IAssetLoadingHandler* handler,
                                                const AssetOptions*   options)
  {
    AssetLoadingTask::Parameters params;
    params.path     = std::move(path);
    params.type     = &type;
    params.handler  = handler;
    params.options  = options;
    params.storage  = &mAssetStorage;
    params.registry = mAssetRegistry.get();

    auto cbCreateAssetInstance =
      [this](const RuntimeClass& type, Path path, Status* status) {
        return this->CreateAssetInstance(type, std::move(path), status);
      };

    TUniquePointer<AssetLoadingTask> task =
      AssetLoadingTask::Create(params, std::move(cbCreateAssetInstance));

    TSharedPointer<Asset> asset = task ? task->GetAsset() : nullptr;
    if (!asset)
    {
      SRX_WARN("[AssetManager] Failed to load {} asset '{}'",
               type.GetName(),
               params.path.generic_string());
      return nullptr;
    }

    switch (mode)
    {
    case ELoadingMode::Sync:
      [[unlikely]] return LoadAssetSync(std::move(task)) ? asset : nullptr;

    case ELoadingMode::Async:
      [[likely]] mWorker->Push(std::move(task));
      return asset;

    default:
      SRX_NOENTRY("invalid loading mode");
      return nullptr;
    }
  }

  AssetCreator::AssetInstance AssetManager::CreateAssetInstance(
    const RuntimeClass& type,
    Path                path,
    Status*             status) const
  {
    SharedLock          lock(mMutex);
    AssetCreator* const creator = FindAssetCreator(type);

    if (creator == nullptr)
    {
      SRX_STATUS_PTR_MSG(status,
                         EStatusCode::Not_Found,
                         "asset creator for the type '{}' not found",
                         type.GetName());
      return std::make_pair(nullptr, nullptr);
    }

    return creator->CreateAssetInstance(std::move(path),
                                        mAssetRegistry.get(),
                                        status);
  }

  void AssetManager::Register(const RuntimeClass&          type,
                              TUniquePointer<AssetCreator> loader)
  {
    SRX_DEBUG("[AssetManager] Register asset creator for '{}' type",
              type.GetName());

    mMutex.lock();
    mAssetFactory[std::addressof<const RuntimeClass>(type)] = std::move(loader);
    mMutex.unlock();
  }

  AssetCreator* AssetManager::FindAssetCreator(
    const RuntimeClass& assetType) const
  {
    auto it = mAssetFactory.find(&assetType);
    return it != mAssetFactory.end() ? it->second.get() : nullptr;
  }
}  // namespace
