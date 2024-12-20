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

#include <type_traits>

#include <Sorex/SxCoreMinimal.h>
#include <Sorex/RuntimeClass.h>

namespace Sorex
{
  /**
   * @class TObjectContainer.
   *
   * @brief TObjectContainer contains and manages the object by their type.
   */
  template<class Type>
  class SRX_API TObjectContainer
  {
    template<typename T>
    using TEnableIf_IsSubclassType =
      typename std::enable_if<std::is_base_of<Type, T>::value, T>::type;

public:
    using Container     = TVector<TUniquePointer<Type>>;
    using Iterator      = typename Container::iterator;
    using ConstIterator = typename Container::const_iterator;

    TObjectContainer(const TObjectContainer&)            = delete;
    TObjectContainer& operator=(const TObjectContainer&) = delete;

    TObjectContainer() = default;
    virtual ~TObjectContainer() { Clear(); }

    Iterator begin() { return mContainer.begin(); }
    Iterator end() { return mContainer.end(); }

    ConstIterator cbegin() const { return mContainer.cbegin(); }
    ConstIterator cend() const { return mContainer.cend(); }

    SRX_INLINE auto rbegin() SRX_NOEXCEPT { return mContainer.rbegin(); }
    SRX_INLINE auto rend() SRX_NOEXCEPT { return mContainer.rend(); }

    /**
     * @param object - the object pointer
     *
     * @return True if the contanier has the object
     */
    SRX_NODISCARD SRX_INLINE bool Contains(const Type* object) const
      SRX_NOEXCEPT;

    /**
     * @return true if the container is empty
     */
    SRX_NODISCARD bool IsEmpty() const SRX_NOEXCEPT
    {
      return mContainer.empty();
    }

    /**
     * @return number of objects that are stored in container
     */
    SRX_NODISCARD size_t GetSize() const SRX_NOEXCEPT
    {
      return mContainer.size();
    }

    /**
     * Move a new object to container. The TObjectContainer owns and manages the
     * object.
     *
     * @param object - move object to container
     * @return pointer to the object or NULL if object isn't valid.
     */
    Type* Add(TUniquePointer<Type>&& object) SRX_NOEXCEPT;

    /**
     * Create and added new object to container. The TObjectContainer owns and
     * manages the object.
     *
     * @param args - arguments to construct new object
     * @return pointer to created object or NULL if object wasn't created.
     */
    template<class T, typename... Args>
    SRX_TYPENAME TEnableIf_IsSubclassType<T>* Add(Args&&... args);

    /**
     * Release the object from container by pointer.
     *
     * @param object - pointer to a removing object
     * @return unique pointer that points to the removed object or NULL if
     * object wasn't found.
     */
    template<class T, class Enable = SRX_TYPENAME TEnableIf_IsSubclassType<T>>
    SRX_NODISCARD TUniquePointer<T> Release(const T* object) SRX_NOEXCEPT;

    /**
     * Release the first found object from container by type.
     *
     * @return unique pointer that points to the removed object or NULL if
     * object wasn't found.
     */
    template<class T, class Enable = SRX_TYPENAME TEnableIf_IsSubclassType<T>>
    SRX_NODISCARD TUniquePointer<T> Release() SRX_NOEXCEPT;

    /**
     * Remove the object from the container and destroy it
     *
     * @return True if element was deleted
     */
    bool Remove(const Type* object) SRX_NOEXCEPT;

    /**
     * Remove all objects with the type from the container and destroy them
     */
    template<class T, class Enable = SRX_TYPENAME TEnableIf_IsSubclassType<T>>
    void RemoveAll() SRX_NOEXCEPT;

    /**
     * Delete all objects and clear container
     */
    void Clear() SRX_NOEXCEPT;

    /**
     * @return first found object by type
     */
    template<class T, class Enable = SRX_TYPENAME TEnableIf_IsSubclassType<T>>
    const T* Get() const SRX_NOEXCEPT;

    /**
     * @return first found object by type
     */
    template<class T>
    SRX_TYPENAME TEnableIf_IsSubclassType<T>* Get() SRX_NOEXCEPT;

    /**
     * @return all found object by type
     */
    template<class T, class Enable = SRX_TYPENAME TEnableIf_IsSubclassType<T>>
    SRX_INLINE void GetAll(TVector<T*>& objects) SRX_NOEXCEPT;

    /**
     * @return all found object by type
     */
    template<class T, class Enable = SRX_TYPENAME TEnableIf_IsSubclassType<T>>
    SRX_INLINE void GetAll(TVector<const T*>& objects) const SRX_NOEXCEPT;

private:
    template<class T>
    void GetAll(TVector<T*>& objects, const RuntimeClass& type) SRX_NOEXCEPT;
    template<class T>
    void GetAll(TVector<const T*>&  objects,
                const RuntimeClass& type) const SRX_NOEXCEPT;

    SRX_INLINE Iterator      Find(const Type* object) SRX_NOEXCEPT;
    SRX_INLINE ConstIterator Find(const Type* object) const SRX_NOEXCEPT;

    SRX_INLINE Iterator      Find(const RuntimeClass& type) SRX_NOEXCEPT;
    SRX_INLINE ConstIterator Find(const RuntimeClass& type) const SRX_NOEXCEPT;
    template<class T>
    SRX_INLINE Iterator Find() SRX_NOEXCEPT;

private:
    Container mContainer;
  };

  template<class Type>
  SRX_INLINE bool TObjectContainer<Type>::Contains(const Type* object) const
    SRX_NOEXCEPT
  {
    return Find(object) != mContainer.cend();
  }

  template<class Type>
  Type* TObjectContainer<Type>::Add(TUniquePointer<Type>&& object) SRX_NOEXCEPT
  {
    if (!object)
      return nullptr;

    Type* result = object.get();
    mContainer.push_back(std::move(object));

    return result;
  }

  template<class Type>
  template<class T, typename... Args>
  typename TObjectContainer<Type>::template TEnableIf_IsSubclassType<T>*
  TObjectContainer<Type>::Add(Args&&... args)
  {
    return static_cast<T*>(Add(MakeUnique<T>(std::forward<Args>(args)...)));
  }

  template<class Type>
  template<class T, class Enable>
  SRX_NODISCARD TUniquePointer<T> TObjectContainer<Type>::Release(
    const T* object) SRX_NOEXCEPT
  {
    if (auto it = Find(object); it != mContainer.end())
    {
      T* const ptr = static_cast<T*>(it->release());
      mContainer.erase(it);
      return TUniquePointer<T>(ptr);
    }

    return nullptr;
  }

  template<class Type>
  template<class T, class Enable>
  SRX_NODISCARD TUniquePointer<T> TObjectContainer<Type>::Release() SRX_NOEXCEPT
  {
    const RuntimeClass& type = GetRuntimeType<T>();
    if (Iterator it = Find(type); it != mContainer.end())
    {
      T* const ptr = static_cast<T*>(it->release());
      mContainer.erase(it);
      return TUniquePointer<T>(ptr);
    }

    return nullptr;
  }

  template<class Type>
  bool TObjectContainer<Type>::Remove(const Type* object) SRX_NOEXCEPT
  {
    if (Iterator it = Find(object); it != mContainer.end())
    {
      mContainer.erase(it);
      return true;
    }

    return false;
  }

  template<class Type>
  template<class T, class Enable>
  void TObjectContainer<Type>::RemoveAll() SRX_NOEXCEPT
  {
    const RuntimeClass& cls = GetRuntimeType<T>();
    mContainer.erase(
      std::remove_if(mContainer.begin(),
                     mContainer.end(),
                     [&](const TUniquePointer<Type>& item) {
                       return item && item->GetRuntimeClass().IsA(cls);
                     }),
      mContainer.end());
  }

  template<class Type>
  void TObjectContainer<Type>::Clear() SRX_NOEXCEPT
  {
    // NOTE: we must delete all object in reverse order.
    // In this case we guarantee that injected dependencies work properly during
    // the objects destruction.
    for (auto it = mContainer.rbegin(); it != mContainer.rend(); ++it)
      it->reset();

    mContainer.clear();
  }

  template<class Type>
  template<class T, class Enable>
  const T* TObjectContainer<Type>::Get() const SRX_NOEXCEPT
  {
    if (ConstIterator it = Find(GetRuntimeType<T>()); it != mContainer.cend())
      return static_cast<T*>(it->get());

    return nullptr;
  }

  template<class Type>
  template<class T>
  typename TObjectContainer<Type>::template TEnableIf_IsSubclassType<T>*
  TObjectContainer<Type>::Get() SRX_NOEXCEPT
  {
    if (Iterator it = Find(GetRuntimeType<T>()); it != mContainer.end())
      return static_cast<T*>(it->get());

    return nullptr;
  }

  template<class Type>
  template<class T, class Enable>
  SRX_INLINE void TObjectContainer<Type>::GetAll(
    TVector<const T*>& objects) const SRX_NOEXCEPT
  {
    return GetAll<T>(objects, GetRuntimeType<T>());
  }

  template<class Type>
  template<class T, class Enable>
  SRX_INLINE void TObjectContainer<Type>::GetAll(TVector<T*>& objects)
    SRX_NOEXCEPT
  {
    return GetAll<T>(objects, GetRuntimeType<T>());
  }

  template<class Type>
  template<class T>
  void TObjectContainer<Type>::GetAll(TVector<const T*>&  objects,
                                      const RuntimeClass& type) const
    SRX_NOEXCEPT
  {
    static_assert(std::is_base_of<Type, T>::value,
                  "[TObjectContainer] GetAll objects of invalid runtime class");

    objects.clear();
    objects.reserve(mContainer.size());

    for (ConstIterator it = mContainer.cbegin(); it != mContainer.cend(); ++it)
    {
      if (const TUniquePointer<Type>& item = (*it);
          item && item->GetRuntimeClass().IsA(type))
        objects.push_back(static_cast<const T*>(item.get()));
    }
  }

  template<class Type>
  template<class T>
  void TObjectContainer<Type>::GetAll(TVector<T*>&        objects,
                                      const RuntimeClass& type) SRX_NOEXCEPT
  {
    static_assert(std::is_base_of<Type, T>::value,
                  "[TObjectContainer] GetAll objects of invalid runtime class");

    objects.clear();
    objects.reserve(mContainer.size());

    for (Iterator it = mContainer.begin(); it != mContainer.end(); ++it)
    {
      if (const TUniquePointer<Type>& item = (*it);
          item && item->GetRuntimeClass().IsA(type))
        objects.push_back(static_cast<T*>(item.get()));
    }
  }

  template<class Type>
  SRX_INLINE typename TObjectContainer<Type>::Iterator
  TObjectContainer<Type>::Find(const Type* object) SRX_NOEXCEPT
  {
    return std::find_if(mContainer.begin(),
                        mContainer.end(),
                        [object](const TUniquePointer<Type>& element) {
                          return element.get() == object;
                        });
  }

  template<class Type>
  SRX_INLINE typename TObjectContainer<Type>::ConstIterator
  TObjectContainer<Type>::Find(const Type* object) const SRX_NOEXCEPT
  {
    return std::find_if(mContainer.cbegin(),
                        mContainer.cend(),
                        [object](const TUniquePointer<Type>& element) {
                          return element.get() == object;
                        });
  }

  template<class Type>
  SRX_INLINE typename TObjectContainer<Type>::Iterator
  TObjectContainer<Type>::Find(const RuntimeClass& type) SRX_NOEXCEPT
  {
    return std::find_if(mContainer.begin(),
                        mContainer.end(),
                        [&type](const TUniquePointer<Type>& element) {
                          return element->GetRuntimeClass().IsA(type);
                        });
  }

  template<class Type>
  SRX_INLINE typename TObjectContainer<Type>::ConstIterator
  TObjectContainer<Type>::Find(const RuntimeClass& type) const SRX_NOEXCEPT
  {
    return std::find_if(mContainer.cbegin(),
                        mContainer.cend(),
                        [&type](const TUniquePointer<Type>& item) {
                          return item->GetRuntimeClass().IsA(type);
                        });
  }

  template<class Type>
  template<class T>
  SRX_INLINE typename TObjectContainer<Type>::Iterator
  TObjectContainer<Type>::Find() SRX_NOEXCEPT
  {
    return Find(GetRuntimeType<T>());
  }
}  // namespace Sorex
