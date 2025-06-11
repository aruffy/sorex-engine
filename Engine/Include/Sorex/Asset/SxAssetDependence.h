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
  struct AssetDependence final
  {
    Path                path;
    const RuntimeClass& type;

    TSharedPointer<Asset> asset;

    SRX_INLINE AssetDependence(const RuntimeClass& aType,
                               Path                aPath) SRX_NOEXCEPT
      : path(std::move(aPath))
      , type(aType)
      , asset(nullptr)
    {}

    bool SetAsset(TSharedPointer<Asset> aAsset)
    {
      if (!aAsset)
      {
        asset.reset();
        return true;
      }

      if (!aAsset->GetRuntimeClass().IsA(type))
      {
        SRX_NOENTRY("invalid asset type");
        return false;
      }

      SRX_CHECK(!asset);
      asset = std::move(aAsset);
      return true;
    }

    bool operator==(const AssetDependence& other) const noexcept
    {
      return other.type == type && other.path == path;
    }

    bool operator!=(const AssetDependence& other) const noexcept
    {
      return !(*this == other);
    }
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
      const RuntimeClass& type     = GetRuntimeType<T>();
      const hash_t        rttiHash = type.GetHash();
      // FIXME: PATH
      if (mResources[rttiHash]
            .emplace(std::make_pair(name, AssetDependence(type, Path(name))))
            .second)
      {
        mSize++;
        return true;
      }

      return false;
    }

    template<typename T>
      requires std::is_base_of_v<Asset, T>
    TSharedPointer<T> GetAsset(const String& name) const
    {
      const hash_t rttiHash = GetRuntimeType<T>().GetHash();
      auto         dictIt   = mResources.find(rttiHash);

      if (dictIt == mResources.end())
        return nullptr;

      if (auto it = dictIt->second.find(name); it != dictIt->second.end())
      {
        if (TSharedPointer<Asset> asset = it->second.asset)
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
        if (dep.asset)
          assets.push_back(std::move(std::static_pointer_cast<T>(dep.asset)));
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
    size_t                                              mSize = 0;
    THashMap<hash_t, THashMap<String, AssetDependence>> mResources;
  };
}  // namespace
