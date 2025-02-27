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

namespace Sorex
{
  template<typename T>
  class TObjectCreator
  {
public:
    virtual ~TObjectCreator() = default;

    virtual TUniquePointer<T> Create() const = 0;
    TSharedPointer<T>         GetOrCreate();

private:
    TSharedPointer<T> mObject;
  };

  template<typename T, typename Key = String>
  class TObjectFactory final
  {
public:
    static_assert(TIsBaseOf_Value<Object, T>, "invalid object type");
    using Creator = TObjectCreator<T>;

public:
    TObjectFactory() = default;

    void RegisterCreator(const Key& uid, TUniquePointer<Creator> creator);

    template<typename Type, typename... Args>
    void RegisterCreator(const Key& uid, Args&&... args);

    SRX_NODISCARD SRX_INLINE bool HasCreator(const Key& uid) const;

    bool RemoveCreator(const Key& uid);

    void GetCreatorList(TVector<Key>& keys) const;

    TUniquePointer<T> Create(const Key& uid) const;
    TSharedPointer<T> GetOrCreate(const Key& uid);

private:
    THashMap<Key, TUniquePointer<Creator>> mCreators;
  };

  template<typename T>
  TSharedPointer<T> TObjectCreator<T>::GetOrCreate()
  {
    if (!mObject)
      mObject = Create();

    return mObject;
  }

  template<typename T, typename Key>
  void TObjectFactory<T, Key>::RegisterCreator(const Key&              uid,
                                               TUniquePointer<Creator> creator)
  {
    SRX_CHECK(creator);

    if (creator)
      mCreators[uid] = std::move(creator);
  }

  template<typename T, typename Key>
  template<typename Type, typename... Args>
  void TObjectFactory<T, Key>::RegisterCreator(const Key& uid, Args&&... args)
  {
    static_assert(std::is_base_of_v<Creator, Type>, "invalid creator type");
    mCreators[uid] = MakeUnique<Type>(std::forward<Args>(args)...);
  }

  template<typename T, typename Key>
  bool TObjectFactory<T, Key>::RemoveCreator(const Key& uid)
  {
    if (auto it = mCreators.find(uid); it != mCreators.end())
    {
      const bool bDeleted = (it->second != nullptr);
      mCreators.erase(it);
      return bDeleted;
    }

    return false;
  }

  template<typename T, typename Key>
  SRX_NODISCARD SRX_INLINE bool TObjectFactory<T, Key>::HasCreator(
    const Key& uid) const
  {
    return mCreators.count(uid);
  }

  template<typename T, typename Key>
  void TObjectFactory<T, Key>::GetCreatorList(TVector<Key>& keys) const
  {
    size_t indx = 0;
    keys.resize(_creators.size());
    for (const auto& [uid, _] : _creators)
      keys[indx++] = uid;
  }

  template<typename T, typename Key>
  TUniquePointer<T> TObjectFactory<T, Key>::Create(const Key& uid) const
  {
    if (auto it = _creators.find(uid); it != _creators.end())
      return it->second->Create();

    return nullptr;
  }

  template<typename T, typename Key>
  TSharedPointer<T> TObjectFactory<T, Key>::GetOrCreate(const Key& uid)
  {
    if (auto it = _creators.find(uid); it != _creators.end())
      return it->second->GetOrCreate();

    return nullptr;
  }

}  // namespace once

#include <Ruffy/Core/CoreMinimal.h>
#include <Ruffy/Core/TypeTraits.h>

namespace Ruffy
{
  template<typename T>
  class TObjectCreator: public Object
  {
    RFY_RTTI(TObjectCreator, Object)

public:
    virtual ~TObjectCreator() override = default;

    virtual TUniquePointer<T> Create() const = 0;
    TSharedPointer<T>         GetOrCreate();

private:
    TSharedPointer<T> _object;
  };

  template<typename T, typename Key = String>
  class TObjectFactory final
  {
public:
    static_assert(TIsBaseOf_Value<Object, T>, "invalid object type");
    using Creator = TObjectCreator<T>;

public:
    TObjectFactory() = default;

    TObjectFactory(const TObjectFactory& other)            = delete;
    TObjectFactory& operator=(const TObjectFactory& other) = delete;

    void RegisterCreator(const Key&              uid,
                         TUniquePointer<Creator> creator) RFY_NOEXCEPT;

    template<typename Type, typename... Args>
    void RegisterCreator(const Key& uid, Args&&... args) RFY_NOEXCEPT;

    RFY_NODISCARD bool HasCreator(const Key& uid) const RFY_NOEXCEPT;

    bool RemoveCreator(const String& uid) RFY_NOEXCEPT;

    void GetCreatorList(TVector<Key>& keys) const RFY_NOEXCEPT;

    TUniquePointer<T> Create(const Key& uid) const;
    TSharedPointer<T> GetOrCreate(const Key& uid);

private:
    THashMap<Key, TUniquePointer<Creator>> _creators;
  };

  template<typename T>
  TSharedPointer<T> TObjectCreator<T>::GetOrCreate()
  {
    if (!_object)
      _object = Create();

    return _object;
  }

  template<typename T, typename Key>
  void TObjectFactory<T, Key>::RegisterCreator(const Key&              uid,
                                               TUniquePointer<Creator> creator)
    RFY_NOEXCEPT
  {
    if (!creator)
      return;

    _creators[uid] = std::move(creator);
  }

  template<typename T, typename Key>
  template<typename Type, typename... Args>
  void TObjectFactory<T, Key>::RegisterCreator(const Key& uid,
                                               Args&&... args) RFY_NOEXCEPT
  {
    static_assert(TIsBaseOf_Value<Creator, Type>, "invalid creator type");

    _creators[uid] = MakeUnique<Type>(std::forward<Args>(args)...);
  }

  template<typename T, typename Key>
  bool TObjectFactory<T, Key>::RemoveCreator(const String& uid) RFY_NOEXCEPT
  {
    if (auto it = _creators.find(uid); it != _creators.end())
    {
      const bool bDeleted = (it->second != nullptr);
      _creators.erase(it);
      return bDeleted;
    }

    return false;
  }

  template<typename T, typename Key>
  RFY_NODISCARD bool TObjectFactory<T, Key>::HasCreator(const Key& uid) const
  {
    return _creators.count(uid);
  }

  template<typename T, typename Key>
  void TObjectFactory<T, Key>::GetCreatorList(TVector<Key>& keys) const
  {
    keys.resize(mCreators.size());
    std::transform(mCreators.begin(),
                   mCreators.end(),
                   keys.begin(),
                   [](const auto& pair) { return pair.first; });
  }

  template<typename T, typename Key>
  TUniquePointer<T> TObjectFactory<T, Key>::Create(const Key& uid) const
  {
    if (auto it = mCreators.find(uid); it != mCreators.end() && it->second)
      return it->second->Create();

    return nullptr;
  }

  template<typename T, typename Key>
  TSharedPointer<T> TObjectFactory<T, Key>::GetOrCreate(const Key& uid)
  {
    if (auto it = mCreators.find(uid); it != mCreators.end() && it->second)
      return it->second->GetOrCreate();

    return nullptr;
  }

}  // namespace
