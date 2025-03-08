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

#include <Sorex/Utils/SxString.h>

#include "SxAsset.h"

namespace Sorex::Resource
{
  /**
   * @class AssetRegistry.
   *
   * The idea of this class to be base for resource cache if needed.
   * If you need have a resource cache or controll resources liftime you can use
   * this as a base class for it.
   *
   * It responsible for unload/delete unused resources and keep resource
   * according the cache type.
   *
   * @see AssetManager
   */
  class AssetRegistry
  {
    SRX_RTTI_BASE(Resource::AssetRegistry);

public:
    enum Cache
    {
      kDefaultCache = 0,
      kSceneCache,
      kDirectorCache,
      kGlobalCache
    };

public:
    virtual ~AssetRegistry() = default;

    AssetRegistry(const AssetRegistry& other)            = delete;
    AssetRegistry& operator=(const AssetRegistry& other) = delete;

    virtual void Update(const float deltaTime) {}

    /**
     * @brief Reset caches according to the cache parameter.
     *  This should reset all chaches which have the same or lower type than
     * parameter value.
     *
     * @param cache - chaches with the same type or lower that should be clean.
     */
    virtual void Reset(const int cache, bool bUnload = false) = 0;

    template<typename T>
    SRX_INLINE std::enable_if_t<std::is_base_of_v<Asset, T>, Status> Register(
      TSharedPointer<T> asset,
      int               cache = kDefaultCache)
    {
      return RegisterAsset(GetRuntimeType<T>(), std::move(asset), cache);
    }

    template<typename T>
      requires std::is_base_of_v<Asset, T>
    SRX_INLINE TSharedPointer<T> Get(StringView name)
    {
      return std::static_pointer_cast<T>(GetAsset(GetRuntimeType<T>(), name));
    }

protected:
    virtual TSharedPointer<Asset> GetAsset(const RuntimeClass& type,
                                           StringView          name) = 0;
    virtual Status                RegisterAsset(const RuntimeClass&   type,
                                                TSharedPointer<Asset> asset,
                                                int                   cache)  = 0;
  };
}  // namespace
