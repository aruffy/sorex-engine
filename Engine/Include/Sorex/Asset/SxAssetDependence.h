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

#include "SxAsset.h"

namespace Sorex::Resource
{
  class AssetDependence final
  {
public:
    SRX_INLINE AssetDependence(const RuntimeClass& type,
                               StringView          name) SRX_NOEXCEPT
      : mName(name)
      , mType(type)
    {}

    AssetDependence(const AssetDependence& other)            = default;
    AssetDependence& operator=(const AssetDependence& other) = delete;

    AssetDependence(AssetDependence&& other)            = default;
    AssetDependence& operator=(AssetDependence&& other) = delete;

    const RuntimeClass& GetType() const { return mType; }
    const String&       GetName() const { return mName; }

    bool IsEmpty() const { return mAsset == nullptr; }

    bool SetAsset(TSharedPointer<Asset> asset)
    {
      if (asset && asset->GetRuntimeClass().IsA(mType))
      {
        SRX_CHECK(!mAsset);
        mAsset = std::move(asset);
        return true;
      }

      return false;
    }

    TSharedPointer<Asset> GetAsset() const { return mAsset; }

    bool operator==(const AssetDependence& other) const noexcept
    {
      return other.mType == mType && other.mName == mName;
    }
    bool operator!=(const AssetDependence& other) const noexcept
    {
      return !(*this == other);
    }

private:
    const String        mName;
    const RuntimeClass& mType;

    TSharedPointer<Asset> mAsset;
  };

  class AssetDependencies final
  {
public:
    bool   IsEmpty() const { return mResources.empty(); }
    size_t GetSize() const { return mSize; }

    template<typename T>
      requires std::is_base_of_v<Asset, T>
    bool Push(const String& name)
    {
      const RuntimeClass* const rt = GetRuntimeType<T>();
      if (mResources[rt].emplace(name, AssetDependence(*rt, name)).second)
      {
        mSize++;
        return true;
      }

      return false;
    }

    template<typename T>
      requires std::is_base_of_v<Asset, T>
    TUniquePointer<T> GetAsset(const String& name) const
    {
      const RuntimeClass* rt     = GetRuntimeType<T>();
      auto                dictIt = mResources.find(rt);

      if (dictIt == mResources.end())
        return nullptr;

      if (auto it = dictIt->second.find(name); it != dictIt->second.end())
      {
        if (TSharedPointer<Asset> asset = it->second.GetAsset())
        {
          SRX_CHECK_MSG(asset->IsA<T>(), "invalid type");
          return std::static_pointer_cast<T>(asset);
        }
      }

      return nullptr;
    }

    template<typename T>
      requires std::is_base_of_v<Asset, T>
    void GetAssets(TVector<T>& assets) const
    {
      assets.clear();
      const RuntimeClass* rt = GetRuntimeType<T>();
      auto                it = mResources.find(rt);

      if (it == mResources.end())
        return;

      assets.reserve(it->second.size());
      for (const auto& [_, dep] : it->second)
      {
        if (TSharedPointer<Asset> asset = dep.GetAsset())
          assets.push_back(std::move(std::static_pointer_cast<T>(asset)));
      }
    }

    void GetAll(TVector<AssetDependence*>& deps)
    {
      deps.clear();
      deps.reserve(mSize);
      for (auto& [_rt, rmap] : mResources)
      {
        for (auto& [_name, rd] : rmap)
          deps.push_back(&rd);
      }
    }

    SRX_INLINE void Clear() { mResources.clear(); }

private:
    size_t mSize = 0;
    // TODO: Flat-Map boost
    THashMap<const RuntimeClass*, THashMap<String, AssetDependence>> mResources;
  };
}  // namespace
