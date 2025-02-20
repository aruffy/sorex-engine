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
#include <Sorex/Containers/SxObjectContainer.h>

#include "SxAssetRegistry.h"

namespace Sorex::Resource
{
  class AssetOption
  {
    SRX_RTTI_BASE(Resource::AssetOption);

public:
    virtual ~AssetOption() = default;
  };

  /**
   * @brief AssetOptions that can be pass to the asset creator and loader to
   * provide creation/loading parameters;
   *
   */
  class AssetOptions final
  {
    SRX_RTTI_BASE(Resource::AssetOptions);

    using Option = std::variant<bool, uint8, int32, size_t, float, String>;

    template<typename T>
    static constexpr bool TIsVariantMember =
      std::is_variant_memeber<T, Option>::value;

public:
    template<typename T>
      requires TIsVariantMember<T>
    SRX_INLINE bool Emplace(const String& name, T&& value)
    {
      return mOptions.emplace(name, std::forward<T>(value)).second;
    }

    template<typename T>
      requires TIsVariantMember<T>
    SRX_INLINE void Set(const String& name, T&& value)
    {
      mOptions[name] = std::forward<T>(value);
    }

    SRX_INLINE void Remove(const String& name) { mOptions.erase(name); }

    template<typename T>
      requires TIsVariantMember<T>
    const T* Find(const String& name) const
    {
      if (auto it = mOptions.find(name); it != mOptions.end())
      {
        if (std::holds_alternative<T>(it->second))
          return std::addressof<const T>(std::get<T>(it->second));
      }

      return nullptr;
    }

    template<typename T>
      requires TIsVariantMember<T>
    const T GetOrDefault(const String& name, const T& defaultValue) const
    {
      if (auto it = mOptions.find(name); it != mOptions.end())
      {
        if (std::holds_alternative<T>(it->second))
          return std::get<T>(it->second);
      }

      return defaultValue;
    }

    void SetDefaultRegistryCache(const AssetRegistry::Cache cache)
    {
      mDefaultRegistryCache = cache;
    }

    template<Concept::RuntimeClass T>
    AssetRegistry::Cache GetAssetRegistryCache() const
    {
      if (auto it = mRegistryCache.find(&GetRuntimeClass<T>());
          it != mRegistryCache.end())
        return it->second;

      return mDefaultRegistryCache;
    }

    template<Concept::RuntimeClass T>
    void SetAssetRegistryCache(const AssetRegistry::Cache cache)
    {
      mRegistryCache[&GetRuntimeClass<T>()] = cache;
    }

public:
    TObjectContainer<AssetOption> mParameters;

private:
    THashMap<String, Option>                            mOptions;
    THashMap<const RuntimeClass*, AssetRegistry::Cache> mRegistryCache;
    AssetRegistry::Cache                                mDefaultRegistryCache =
      AssetRegistry::Cache::kDefaultCache;
  };
}
