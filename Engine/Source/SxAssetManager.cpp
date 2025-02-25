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
      // TODO: Add while looping to wait for the task to complete
      if (action == ETaskAction::Await)
        status =
          SRX_STATUS_MSG(EStatusCode::Not_Supported,
                         "asset sync loading mode doesn't support awaiting");
      else
        status = SRX_STATUS_MSG(EStatusCode::Canceled, "asset loading failed");

      task->Shutdown();
      return status;
    }

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

  Status AssetManager::Initialize()
  {
    mWorker = std::make_unique<TaskWorker>("AssetLoadingThread");
    return SRX_STATUS(mWorker->Start());
  }

  void AssetManager::Shutdown()
  {
    if (mWorker)
    {
      mWorker->Stop();
      mWorker.reset();
    }
  }

  void AssetManager::Update(const float deltaTime)
  {
    SRX_CHECK(mWorker);

    if (mWorker->HasCompletedTask())
    {
      if (auto task = mWorker->Pop())
      {
        if (!task->Finalize())
          task->Shutdown();
      }
    }

    if (mAssetRegistry)
      mAssetRegistry->Update(deltaTime);
  }

  TUniquePointer<Asset> AssetManager::LoadAsset(const RuntimeClass&   type,
                                                StringView            name,
                                                ELoadingMode          mode,
                                                IAssetLoadingHandler* handler,
                                                const AssetOptions*   options)
  {
    LoadingTask::Parameters params;
    params.name     = name;
    params.type     = &type;
    params.handler  = handler;
    params.options  = options;
    params.storage  = &_storage;
    params.registry = _registry.get();

    TUniquePointer<LoadingTask> task = LoadingTask::Create(
      params,
      [this, options](const RuntimeType& type, StringView name, Error* error) {
        return CreateAssetLoader(type, name, options, error);
      });

    TRef<Asset> asset = task ? task->GetAsset() : nullptr;
    if (!asset)
    {
      RFY_NOENTRY("task creation failed");
      return nullptr;
    }

    switch (mode)
    {
    case ELoadingMode::Sync:
      // The sync loading should provide loaded resource or null
      return s_LoadSync(std::move(task)) ? asset : nullptr;

    case ELoadingMode::Async:
      _worker->Push(std::move(task));
      return asset;

    case ELoadingMode::Deferred:
      asset->Defer(
        MakeUnique<DeferredAssetActivator>(_worker.get(), std::move(task)));
      return asset;

    default:
      RFY_NOENTRY("invalid loading mode");
      return nullptr;
    }
  }


}  // namespace
