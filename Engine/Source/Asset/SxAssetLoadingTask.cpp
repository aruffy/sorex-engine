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

#include "SxAssetLoadingTask.h"

namespace Sorex::Resource
{
  AssetLoadingTask::AssetLoadingTask(ETaskPriority priority)
    : Task(priority)
  {}

  TUniquePointer<AssetLoadingTask> AssetLoadingTask::Create(
    const Parameters&   params,
    CreateAssetCallback callback)
  {
    if (!params.storage || !params.type || !callback)
    {
      SRX_NOENTRY("invalid asset loading task parameters");
      return nullptr;
    }

    auto task = new AssetLoadingTask(params.priority);

    task->mCommonCtx.storage  = params.storage;
    task->mCommonCtx.registry = params.registry;
    task->mCommonCtx.handler  = params.handler;
    task->mCommonCtx.options  = params.options;
    task->mCommonCtx.status   = SRX_OK;

    task->mContext.name = params.name;
    task->mContext.type = params.type;
    task->mContext.asset =
      callback(*params.type, params.name, &task->mCommonCtx.status);
    task->mContext.common      = &task->mCommonCtx;
    task->mCreateAssetCallback = std::move(callback);

    return TUniquePointer<AssetLoadingTask>(task);
  }

  AssetLoadingTask::~AssetLoadingTask()
  {
    if (mContext.stage != Context::ELoadingStage::Done)
    {
      // @note: uncompleted task can lead that asset will stuck in invalid
      // state;
      SRX_NOENTRY("destroy uncompleted task: Shutdown() wasn't called");
      Shutdown();
    }
  }

  ETaskAction AssetLoadingTask::Execute()
  {
    SRX_DEBUG("[{}] {}: asset '{}' loading",
              GetRuntimeClass().GetName(),
              __FUNCTION__,
              mContext.name);

    const ETaskAction res = Load(mContext);

#ifdef SRX_DEBUG_MEDIUM
    if (res == ETaskAction::Cancel)
      SRX_ASSERT(!mCommonCtx.status.Ok());
#endif

    return res;
  }

  void AssetLoadingTask::Shutdown()
  {
    SRX_DEBUG("[{}] {}: asset '{}' loading",
              GetRuntimeClass().GetName(),
              __FUNCTION__,
              mContext.name);

    // If the asset loading stage is done than:
    // 1. It has already been invalidated
    // 2. The asset was successfully loaded
    // No additional action is needed
    if (mContext.stage == Context::ELoadingStage::Done)
      return;

    if (mCommonCtx.status.Ok())
      mCommonCtx.status = SRX_STATUS_MSG(EStatusCode::Interrupted,
                                         "laoding task was interrupted");

    // TODO: Do we need to keep dependencies here?
    Context::Invalidate(mContext);
  }

  ETaskAction AssetLoadingTask::Resume()
  {
    bool bCanceled = false;
    for (auto it = mCommonCtx.deferred.begin();
         it != mCommonCtx.deferred.end();)
    {
      SRX_CHECK(*it && (*it)->awaiter);

      ETaskAction action = ETaskAction::Continue;
      if (Context* ctx = (*it); ctx && ctx->awaiter)
      {
        if (const Asset* asset = Context::GetAsset(*ctx))
          action = ctx->awaiter->GetAssetStatus(*asset, &mCommonCtx.status);

        if (action == ETaskAction::Cancel)
        {
          bCanceled          = true;
          mCommonCtx.current = ctx->name;
          if (mCommonCtx.status.Ok())
          {
            SRX_NOENTRY("loading failed/canceled: should be error description");
            mCommonCtx.status = SRX_STATUS_MSG(EStatusCode::Interrupted,
                                               "asset '{}' loading canceled",
                                               ctx->name);
          }
          break;
        }
      }

      if (action == ETaskAction::Continue)
        it = mCommonCtx.deferred.erase(it);
      else
        ++it;
    }

    if (bCanceled)
      return ETaskAction::Cancel;

    return mCommonCtx.deferred.empty() ? ETaskAction::Continue
                                       : ETaskAction::Await;
  }

  bool AssetLoadingTask::Context::FinalizeRecursive(Context& ctx)
  {
    SRX_CHECK(ctx.stage == Context::ELoadingStage::Loaded);
    if (ctx.stage != Context::ELoadingStage::Loaded)
    {
      ctx.common->status = SRX_STATUS_MSG(EStatusCode::Invalid_State,
                                          "asset '{}' loading failed, stage={}",
                                          ctx.name,
                                          static_cast<int>(ctx.stage));
      return false;
    }

    ctx.stage = Context::ELoadingStage::Finalization;
    if (const auto loader = ctx.asset.second.get())
    {
      ctx.common->status =
        loader->Finalize(ctx.common->registry, ctx.dependencies);

      if (!ctx.common->status.Ok())
        return false;
    }

    for (auto& subctx : ctx.subcontexts)
      if (!FinalizeRecursive(subctx))
        return false;

    return true;
  }

  Status AssetLoadingTask::Finalize()
  {
    SRX_DEBUG("[{}] {}: asset '{}' loading",
              GetRuntimeClass().GetName(),
              __FUNCTION__,
              mContext.name);

    Context::Finalize(mContext);

    return mCommonCtx.status;
  }

#ifdef SOREX_DEBUG_MEDIUM
  bool AssetLoadingTask::IsValidContextToLoad(const Context& ctx)
  {
    if (!ctx.common || !ctx.common->storage)
      return false;

    if (ctx.name.empty() || ctx.type == nullptr)
      return false;

    if (ctx.stage >= Context::ELoadingStage::Loading)
      return false;

    return ctx.dependencies.IsEmpty() && ctx.subcontexts.empty();
  }
#endif

  ETaskAction AssetLoadingTask::Load(Context& ctx)
  {
#ifdef SOREX_DEBUG_MEDIUM
    SRX_ASSERT(IsValidContextToLoad(ctx));
#endif

    SRX_DEBUG(
      "[{}] {} loading <{}> asset '{}'",
      GetRuntimeClass().GetName(),
      (ctx.stage == Context::ELoadingStage::None ? "Start" : "Continue"),
      ctx.type->GetName(),
      ctx.name);

    ctx.common->current = ctx.name;
    Status& status      = ctx.common->status;

    if (ctx.name.empty())
    {
      status = SRX_STATUS_MSG(EStatusCode::Invalid_Argument,
                              "asset '{}' name is empty",
                              ctx.type->GetName());
      return ETaskAction::Cancel;
    }

    if (!ctx.asset.first)
      ctx.asset = mCreateAssetCallback(*ctx.type, ctx.name, &status);

    Asset* const asset = ctx.asset.first.get();
    if (!asset)
      return ETaskAction::Cancel;

    asset->SetState(EAssetState::Loading);
    AssetLoader* const loader = ctx.asset.second.get();
    if (ctx.asset.second == nullptr)
    {
      // @NOTE: see the @CreateAssetInstance: the asset is ready to use
      ctx.stage = Context::ELoadingStage::Loaded;
      return ETaskAction::Continue;
    }

    // @NOTE: the resource hasn't made a preload stage yet
    if (ctx.stage == Context::ELoadingStage::None)
    {
      TVector<String> missingFiles;
      status = loader->Preload(*ctx.common->storage,
                               ctx.common->registry,
                               missingFiles);
      if (!status.Ok())
        return ETaskAction::Cancel;

      ctx.stage = Context::ELoadingStage::Preload;
      if (!missingFiles.empty())
      {
        ETaskAction taskAction = ETaskAction::Cancel;
        if (auto handler = ctx.common->handler)
        {
          auto [action, awaiter] =
            handler->HandleMissingFiles(*ctx.common->storage,
                                        ctx.common->registry,
                                        asset,
                                        missingFiles);

          taskAction = action;
          if (taskAction == ETaskAction::Await)
          {
            SRX_CHECK_MSG(awaiter,
                          "resource awaiting without an awaiter object");

            if (awaiter)
            {
              ctx.awaiter = std::move(awaiter);
              ctx.common->deferred.push_front(&ctx);
              ctx.stage = Context::ELoadingStage::Waiting;
              return ETaskAction::Await;
            }
          }
        }

        if (taskAction == ETaskAction::Cancel)
        {
          status = SRX_STATUS_MSG(EStatusCode::No_Data,
                                  "missing <{}> files",
                                  missingFiles.size());
          return ETaskAction::Cancel;
        }
      }
    }

    ctx.stage = Context::ELoadingStage::Loading;
    status =
      loader->Load(*ctx.common->storage, ctx.common->options, ctx.dependencies);

    if (!status.Ok())
      return ETaskAction::Cancel;

    bool bAwait = false;
    if (!ctx.dependencies.IsEmpty())
    {
      TVector<AssetDependence*> dependencies;
      ctx.dependencies.GetAll(dependencies);
      for (AssetDependence* const dependence : dependencies)
      {
        if (!dependence || dependence->GetName().empty())
          continue;

        ctx.subcontexts.emplace_back(ctx,
                                     dependence->GetType(),
                                     dependence->GetName());

        Context&          depctx = ctx.subcontexts.back();
        const ETaskAction result = Load(depctx);

        if (result == ETaskAction::Cancel)
          return ETaskAction::Cancel;

        SRX_CHECK(depctx.asset.first);
        // @TODO: Should be a shared pointer/weak
        // pointer/raw?
        dependence->SetAsset(depctx.asset.first->shared_from_this());
        if (result == ETaskAction::Await)
          bAwait = true;
      }
    }

    ctx.stage = Context::ELoadingStage::Loaded;
    return bAwait ? ETaskAction::Await : ETaskAction::Continue;
  }

  AssetLoadingTask::Context::Context(Context&            parent,
                                     const RuntimeClass& aType,
                                     StringView          aName)
    : common(parent.common)
    , parent(&parent)
    , name(aName)
    , type(&aType)
  {}

  AssetLoadingTask::Context& AssetLoadingTask::Context::GetRoot(Context& ctx)
  {
    Context* ptr = &ctx;
    while (ptr->parent)
      ptr = ptr->parent;

    return *ptr;
  }

  Asset* AssetLoadingTask::Context::GetAsset(Context& ctx)
  {
    return ctx.asset.first.get();
  }

  void AssetLoadingTask::Context::SetStateRecursive(Context&    ctx,
                                                    EAssetState state)
  {
    if (state == EAssetState::Loaded || state == EAssetState::Invalid)
      ctx.stage = Context::ELoadingStage::Done;

    if (Asset* asset = Context::GetAsset(ctx))
      asset->SetState(state);

    for (auto& subctx : ctx.subcontexts)
      SetStateRecursive(subctx, state);
  }

  void AssetLoadingTask::Context::Finalize(Context& ctx)
  {
    SRX_CHECK_MSG(ctx.parent == nullptr,
                  "finilize should be called for root only");

    Context& root = Context::GetRoot(ctx);
    if (!ctx.common->status.Ok() || !FinalizeRecursive(root))
    {
      Invalidate(root);
      return;
    }

    SRX_INFO("[{}] Asset <{}> '{}' loaded",
             GetTypeName<AssetLoadingTask>(),
             root.type->GetName(),
             root.name);

    SetStateRecursive(root, EAssetState::Loaded);

    if (auto handler = root.common->handler)
      handler->OnAssetLoaded(ctx.common->registry, GetAsset(root));
  }

  void AssetLoadingTask::Context::Invalidate(Context& ctx)
  {
    SRX_CHECK_MSG(!ctx.common->status.Ok(),
                  "invalidate loading context without error description");

    Context& root = Context::GetRoot(ctx);
    SRX_ERROR("Asset <{}> '{}' loading failed: {}",
              root.type->GetName(),
              root.name,
              root.common->status.ToString());

    if (auto handler = root.common->handler)
      handler->OnAssetLoadingFailed(ctx.common->current,
                                    GetAsset(root),
                                    ctx.common->status);

    SetStateRecursive(root, EAssetState::Invalid);
  }
}  // namespace
