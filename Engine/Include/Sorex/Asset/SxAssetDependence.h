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

  class Dependencies final
  {
public:
    bool   IsEmpty() const { return _resources.empty(); }
    size_t GetSize() const { return _size; }

    template<typename T>
    bool Push(const String& name)
    {
      const RuntimeType* rt = GetRuntimeType<T>();
      if (_resources[rt].emplace(name, Dependence(*rt, name)).second)
      {
        _size++;
        return true;
      }

      return false;
    }

    template<typename T>
    TRef<T> GetAsset(const String& name) const
    {
      const RuntimeType* rt     = GetRuntimeType<T>();
      auto               dictIt = _resources.find(rt);
      if (dictIt == _resources.end())
        return nullptr;

      if (auto it = dictIt->second.find(name); it != dictIt->second.end())
      {
        TRef<Asset> asset = it->second.GetAsset();
        if (asset == nullptr)
          return nullptr;

        return std::static_pointer_cast<T>(asset);
      }

      return nullptr;
    }

    template<typename T>
    void GetAssets(TVector<T>& assets) const
    {
      assets.clear();
      const RuntimeType* rt = GetRuntimeType<T>();
      auto               it = _resources.find(rt);

      if (it == _resources.end())
        return;

      assets.reserve(it->second.size());
      for (const auto& [_, dep] : it->second)
      {
        TRef<Asset> asset = dep.GetAsset();
        if (!asset)
          continue;

        return assets.push_back(std::move(std::static_pointer_cast<T>(asset)));
      }
    }

    void GetAll(TVector<Dependence*>& deps) RFY_NOEXCEPT
    {
      deps.clear();
      deps.reserve(_size);
      for (auto& [_, rmap] : _resources)
      {
        for (auto& [name, rd] : rmap)
          deps.push_back(&rd);
      }
    }

    inline void Clear() { _resources.clear(); }

private:
    template<typename T>
    static const RuntimeType* GetRuntimeType() noexcept
    {
      return std::addressof<const RuntimeType>(TypeId<T>());
    }

private:
    size_t                                                     _size = 0;
    THashMap<const RuntimeType*, THashMap<String, Dependence>> _resources;
  };
}  // namespace
