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
    int32 Push(const Path& path)
    {
      const RuntimeClass&       type     = GetRuntimeType<T>();
      const hash_t              rttiHash = type.GetHash();
      TVector<AssetDependence>& deps     = mResources[rttiHash];
      const int32               index    = static_cast<int32>(deps.size());

      deps.emplace_back(type, path);
      mSize++;

      return index;
    }

    template<typename T>
      requires std::is_base_of_v<Asset, T>
    TSharedPointer<T> GetAsset(const int32 index) const
    {
      const hash_t rttiHash = GetRuntimeType<T>().GetHash();
      auto         dictIt   = mResources.find(rttiHash);

      if (dictIt == mResources.end())
        return nullptr;

      if (index < 0 || index >= static_cast<int32>(dictIt->second.size()))
      {
        SRX_NOENTRY(__FILE__ "invalid asset index");
        return nullptr;
      }

      const AssetDependence& dep = dictIt->second[index];
      if (dep.asset)
      {
        SRX_CHECK_MSG(dep.asset->IsA<T>(), "invalid type");
        return std::static_pointer_cast<T>(dep.asset);
      }

      return nullptr;
    }

    template<typename T>
      requires std::is_base_of_v<Asset, T>
    void GetAssets(TVector<const T*>& assets) const
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
          assets.push_back(std::move(static_cast<const T*>(dep.asset.get())));
      }
    }

    void GetAll(TVector<AssetDependence*>& deps)
    {
      deps.clear();
      deps.reserve(mSize);
      for (auto& [_, vec] : mResources)
      {
        for (AssetDependence& dep : vec)
          deps.push_back(&dep);  // cppcheck-suppress useStlAlgorithm
      }
    }

    SRX_INLINE void Clear() { mResources.clear(); }

private:
    size_t mSize = 0;
    // NOTE: key - hash_t RTTI class hash
    THashMap<hash_t, TVector<AssetDependence>> mResources;
  };
}  // namespace Sorex::Resource
