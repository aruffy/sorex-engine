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

#include <Sorex/SxTypes.h>
#include <Sorex/SxPlatform.h>
#include <Sorex/SxAssert.h>

namespace Sorex
{
  /**
   * @brief Container stores listener in Observer programming pattern.
   *
   * Allows to add or remove listeners during the iteration process.
   * After Add/Remove operation iterators stay valid.
   *
   */
  template<typename T>
  class TListenerContainer
  {
    friend class Iterator;

public:
    using Listener = T;

    /**
     * @brief Forward iterator of TListenerContainer container
     */
    class Iterator
    {
      friend class TListenerContainer;

  public:
      Iterator() = default;

      Iterator(const Iterator& other) SRX_NOEXCEPT;
      Iterator& operator=(const Iterator& other) SRX_NOEXCEPT;

      ~Iterator() { Reset(); }

      bool            operator==(const Iterator& rhs) const SRX_NOEXCEPT;
      SRX_INLINE bool operator!=(const Iterator& rhs) const SRX_NOEXCEPT
      {
        return !(*this == rhs);
      }

      Iterator& operator++() SRX_NOEXCEPT;
      Iterator  operator++(int) SRX_NOEXCEPT;

      SRX_INLINE Listener* operator*() const
      {
        return mList->mListeners[mIndex];
      }

      SRX_INLINE Listener** operator->() const
      {
        return &mList->mListeners[mIndex];
      }

  private:
      SRX_INLINE explicit Iterator(TListenerContainer* list) SRX_NOEXCEPT;

      void Init() SRX_NOEXCEPT;
      void Reset() SRX_NOEXCEPT;
      void Next() SRX_NOEXCEPT;

      SRX_INLINE bool IsEnd() const SRX_NOEXCEPT
      {
        return !mList || mIndex >= mList->mListeners.size();
      }

      TListenerContainer* mList  = nullptr;
      size_t              mIndex = 0;
    };

public:
    TListenerContainer() = default;

    TListenerContainer(const TListenerContainer& other)            = delete;
    TListenerContainer& operator=(const TListenerContainer& other) = delete;

    /**
     * @brief Remove all listener from container.
     *
     */
    void Clear() SRX_NOEXCEPT;

    /**
     * @breaf Check if listener already is into container.
     *
     * @param listener - checked listener
     * @return True if container has a listener
     */
    bool Contains(const Listener* listener) const SRX_NOEXCEPT;

    /**
     * @brief Check if container is empty.
     *
     * @return True if container has no listeners.
     */
    bool IsEmpty() const SRX_NOEXCEPT { return mListenersNumber == 0u; }

    /**
     * @return number of listeners which are stored into container.
     */
    size_t GetSize() const SRX_NOEXCEPT { return mListenersNumber; }

    /**
     * @brief Add new listener to the container.
     *
     * @param listener - added listener
     * @return True is listener was successfully added to container.
     */
    bool Add(Listener* listener) SRX_NOEXCEPT;

    /**
     * @brief Remove the listener from the container.
     *
     * @param listener - removed listener
     */
    void Remove(Listener* listener) SRX_NOEXCEPT;

    SRX_INLINE Iterator begin() SRX_NOEXCEPT { return Iterator(this); }
    // cppcheck-suppress functionStatic
    SRX_INLINE Iterator end() SRX_NOEXCEPT { return Iterator(); }

    template<typename Fn>
    SRX_INLINE void Notify(Fn&& callback) const
    {
      for (Listener* listener : (*this))
      {
        SRX_CHECK(listener);  // NOTE: iterator takes valid listener
        callback(*listener);
      }
    }

private:
    void Cleanup() SRX_NOEXCEPT;

    TVector<Listener*> mListeners;

    size_t mIteratorsNumber = 0u;
    size_t mListenersNumber = 0u;

    bool mIsCleabupNeeded = false;
  };

  template<typename T>
  void TListenerContainer<T>::Clear() SRX_NOEXCEPT
  {
    mListenersNumber = 0;
    if (mIteratorsNumber) [[unlikely]]
    {
      mIsCleabupNeeded = true;
      for (size_t i = 0; i < mListeners.size(); ++i)
        mListeners[i] = nullptr;
    }
    else
    {
      mListeners.clear();
    }
  }

  template<typename T>
  bool TListenerContainer<T>::Contains(const Listener* listener) const
    SRX_NOEXCEPT
  {
    if (!listener)
      return false;

    auto it = std::find(mListeners.begin(), mListeners.end(), listener);
    return it != mListeners.end();
  }

  template<typename T>
  bool TListenerContainer<T>::Add(Listener* listener) SRX_NOEXCEPT
  {
    SRX_CHECK(!Contains(listener));
    if (listener && !Contains(listener))
    {
      mListeners.push_back(listener);
      ++mListenersNumber;

      return true;
    }

    return false;
  }

  template<typename T>
  void TListenerContainer<T>::Remove(Listener* listener) SRX_NOEXCEPT
  {
    if (!listener)
      return;

    if (auto it = std::find(mListeners.begin(), mListeners.end(), listener);
        it != mListeners.end())
    {
      if (mIteratorsNumber) [[unlikely]]
      {
        *it              = nullptr;
        mIsCleabupNeeded = true;
      }
      else
      {
        mListeners.erase(it);
      }

      --mListenersNumber;
    }
  }

  template<typename T>
  void TListenerContainer<T>::Cleanup() SRX_NOEXCEPT
  {
    SRX_ASSERT(mIteratorsNumber == 0u);

    if (mIsCleabupNeeded)
    {
      mListeners.erase(
        std::remove(mListeners.begin(), mListeners.end(), nullptr),
        mListeners.end());
      mListenersNumber = mListeners.size();
      mIsCleabupNeeded = false;
    }
  }

  // Iterator
  template<typename T>
  TListenerContainer<T>::Iterator::Iterator(TListenerContainer* list)
    SRX_NOEXCEPT
    : mList(list)
    , mIndex(0u)
  {
    Init();
  }

  template<typename T>
  TListenerContainer<T>::Iterator::Iterator(const Iterator& other) SRX_NOEXCEPT
    : mList(other.mList)
    , mIndex(other.mIndex)
  {
    Init();
  }

  template<typename T>
  typename TListenerContainer<T>::Iterator&
  TListenerContainer<T>::Iterator::operator=(const Iterator& other) SRX_NOEXCEPT
  {
    if (this != &other)
    {
      Reset();

      mIndex = other.mIndex;
      mList  = other.mList;

      Init();
    }

    return *this;
  }

  template<typename T>
  bool TListenerContainer<T>::Iterator::operator==(const Iterator& other) const
    SRX_NOEXCEPT
  {
    if (IsEnd() && other.IsEnd())
      return true;

    return (mIndex == other.mIndex && mList == other.mList);
  }

  template<typename T>
  typename TListenerContainer<T>::Iterator&
  TListenerContainer<T>::Iterator::operator++() SRX_NOEXCEPT
  {
    Next();
    return *this;
  }

  template<typename T>
  typename TListenerContainer<T>::Iterator
  TListenerContainer<T>::Iterator::operator++(int) SRX_NOEXCEPT
  {
    Iterator it = *this;
    Next();
    return it;
  }

  template<typename T>
  void TListenerContainer<T>::Iterator::Init() SRX_NOEXCEPT
  {
    if (mList)
    {
      mList->mIteratorsNumber++;
      if (mIndex < mList->mListeners.size() && !mList->mListeners[mIndex])
        Next();
    }
  }

  template<typename T>
  void TListenerContainer<T>::Iterator::Reset() SRX_NOEXCEPT
  {
    if (mList)
    {
      SRX_ASSERT(mList->mIteratorsNumber > 0);
      mList->mIteratorsNumber--;

      if (mList->mIteratorsNumber == 0u)
        mList->Cleanup();
    }

    mList  = nullptr;
    mIndex = 0;
  }


  template<typename T>
  void TListenerContainer<T>::Iterator::Next() SRX_NOEXCEPT
  {
    if (mList == nullptr)
      return;

    const size_t size = mList->mListeners.size();
    do
    {
      ++mIndex;
    } while (mIndex < size && mList->mListeners[mIndex] == nullptr);
  }
}
