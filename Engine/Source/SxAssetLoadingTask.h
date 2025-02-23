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
#include <Sorex/SxTask.h>
#include <Sorex/Asset/SxAssetLoader.h>
#include <Sorex/Asset/SxAssetHandler.h>
#include <Sorex/Asset/SxAssetOptions.h>

namespace Sorex::Resource
{
  class AssetLoadingTask final: public Task
  {
    SRX_RTTI(Resource::AssetLoadingTask, Task);

public:
    using CreateLoaderCallback = std::function<
      TUniquePointer<AssetLoader>(const RuntimeClass&, StringView, Status*)>;

    struct Parameters
    {
      ETaskPriority priority = ETaskPriority::Medium;

      AssetStorage*  storage  = nullptr;
      AssetRegistry* registry = nullptr;

      IAssetLoadingHandler* handler = nullptr;
      const AssetOptions*   options = nullptr;

      String              name;
      const RuntimeClass* type = nullptr;
    };

public:
    virtual ~AssetLoadingTask() override;

    static TUniquePointer<AssetLoadingTask> Create(
      const Parameters&    params,
      CreateLoaderCallback callback);

    virtual ETaskAction Execute() override;
    virtual ETaskAction Resume() override;
    virtual void        Shutdown() override;
    virtual Status      Finalize() override;

    TSharedPointer<Asset> GetAsset() const
    {
      return mContext.loader ? mContext.loader->GetAsset() : nullptr;
    }
    const Asset* GetAssetPtr() const
    {
      return mContext.loader ? mContext.loader->GetAssetPtr() : nullptr;
    }

private:
    // Context represent the loading state of a resource
    // Each asset may have dependent resource that has it's own loading context
    struct Context
    {
      enum class ELoadingStage
      {
        None,
        Waiting,
        Preload,
        Loading,
        Finalization,
        Done
      };

      struct Common
      {
        IAssetLoadingHandler* handler = nullptr;
        const AssetOptions*   options = nullptr;

        AssetStorage*  storage  = nullptr;
        AssetRegistry* registry = nullptr;

        TList<Context*> deferred;  ///< loading context of postponed assets
        StringView      current;   ///< current loading asset

        Status status;
      };

      Context() = default;
      Context(Context& parent, const RuntimeClass& aType, StringView aName);

      static Context& GetRoot(Context& ctx);
      static Asset*   GetAsset(Context& ctx);

      static void Invalidate(Context& ctx);
      static void Complete(Context& ctx);

      Common*  common = nullptr;
      Context* parent = nullptr;

      String              name;
      const RuntimeClass* type = nullptr;

      ELoadingStage stage = ELoadingStage::None;

      TUniquePointer<AssetLoader>   loader;
      TUniquePointer<IAssetAwaiter> awaiter;

      AssetDependencies dependencies;
      TVector<Context>  subcontexts;  ///< contexts of the dependencies

  private:
      static void SetStateRecursive(Context& ctx, EAssetState state);
    };  // Context

private:
    explicit AssetLoadingTask(ETaskPriority priority);

    ETaskAction Load(Context& ctx);
    bool        FinilizeRecursive(Context& ctx);

#ifdef SRX_DEBUG_MEDIUM
    static bool IsContextValid(const Context& ctx);
#endif

private:
    Context         mContext;  // Root context
    Context::Common mCommonCtx;

    CreateLoaderCallback mCreateLoaderCallback;
  };
}  // namespace
