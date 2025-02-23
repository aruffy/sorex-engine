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

namespace Sorex::Resource
{
  enum class EAssetState : uint8
  {
    Unloaded,  ///< Initial state of a asset or asset was unloaded
    Deferred,  ///< Using for lazy loading mode
    Loading,   ///< Asset is being loaded
    Loaded,    ///< Asset is loaded and ready
    Invalid,   ///< Loading process failed
  };

  String ToString(const EAssetState state) SRX_NOEXCEPT;

  // @class IAssetActivator - activate step to load a deferred asset
  class Asset;
  class IAssetActivator
  {
public:
    virtual ~IAssetActivator() = default;

    /**
     * @brief Activate() is called when deferred asset should be prepared for
     * using.
     *
     * @param asset - awaken asset
     */
    virtual Status Activate(const Asset* asset) = 0;
  };

  class Asset: public std::enable_shared_from_this<Asset>
  {
    SRX_RTTI_BASE(Resource::Asset);

public:
    explicit Asset(StringView name) SRX_NOEXCEPT;
    virtual ~Asset() { mActivator.reset(); }

    Asset(const Asset& other)            = delete;
    Asset& operator=(const Asset& other) = delete;

    /**
     * @brief Create asset reference from this.
     *
     * @note: this asset must be managed by shared pointer.
     *
     * @return shared pointer to this asset
     */
    TSharedPointer<Asset> GetRef() { return shared_from_this(); }

    /**
     * @brief Retrieve name of the asset.
     *
     * @return asset name string
     */
    const String& GetName() const { return mName; }

    EAssetState GetState() const SRX_NOEXCEPT
    {
      return SRX_ATOMIC_LOAD(mState);
    }
    void SetState(EAssetState state) SRX_NOEXCEPT
    {
      SRX_TRACE("Asset '{}' state changed: {} -> {}",
                mName,
                ToString(GetState()),
                ToString(state));
      SRX_ATOMIC_STORE(mState, state);
    }

    void Defer(TSharedPointer<IAssetActivator> activator) SRX_NOEXCEPT;

    bool              IsReady() const;
    SRX_INLINE Status Unload();

protected:
    virtual Status OnUnload() { return SRX_OK; }

private:
    String                   mName;
    std::atomic<EAssetState> mState;

    mutable TSharedPointer<IAssetActivator> mActivator;
  };

  SRX_INLINE Status Asset::Unload()
  {
    const auto status = OnUnload();
    const auto state =
      status.Ok() ? EAssetState::Unloaded : EAssetState::Invalid;
    SetState(state);

    return status;
  }

  using AssetPtr = TSharedPointer<Asset>;
}  // namespace

using SxAsset = Sorex::Resource::Asset;
