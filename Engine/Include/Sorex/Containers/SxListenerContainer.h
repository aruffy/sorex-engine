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
        return _list->_listeners[_index];
      }

      SRX_INLINE Listener** operator->() const
      {
        return &_list->_listeners[_index];
      }

  private:
      SRX_INLINE explicit Iterator(TListenerContainer* list) SRX_NOEXCEPT;

      void Init() SRX_NOEXCEPT;
      void Reset() SRX_NOEXCEPT;
      void Next() SRX_NOEXCEPT;

      SRX_INLINE bool IsEnd() const SRX_NOEXCEPT
      {
        return !_list || _index >= _list->_listeners.size();
      }

      TListenerContainer* _list  = nullptr;
      size_t              _index = 0;
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
    bool IsEmpty() const SRX_NOEXCEPT { return _listenersNumber == 0u; }

    /**
     * @return number of listeners which are stored into container.
     */
    size_t GetSize() const SRX_NOEXCEPT { return _listenersNumber; }

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

    TVector<Listener*> _listeners;

    size_t _iteratorsNumber = 0u;
    size_t _listenersNumber = 0u;

    bool _isCleanupNeeded = false;
  };

  template<typename T>
  void TListenerContainer<T>::Clear() SRX_NOEXCEPT
  {
    _listenersNumber = 0;
    if (_iteratorsNumber) [[unlikely]]
    {
      _isCleanupNeeded = true;
      for (size_t i = 0; i < _listeners.size(); ++i)
        _listeners[i] = nullptr;
    }
    else
    {
      _listeners.clear();
    }
  }

  template<typename T>
  bool TListenerContainer<T>::Contains(const Listener* listener) const
    SRX_NOEXCEPT
  {
    if (!listener)
      return false;

    auto it = std::find(_listeners.begin(), _listeners.end(), listener);
    return it != _listeners.end();
  }

  template<typename T>
  bool TListenerContainer<T>::Add(Listener* listener) SRX_NOEXCEPT
  {
    SRX_CHECK(!Contains(listener));
    if (listener && !Contains(listener))
    {
      _listeners.push_back(listener);
      ++_listenersNumber;

      return true;
    }

    return false;
  }

  template<typename T>
  void TListenerContainer<T>::Remove(Listener* listener) SRX_NOEXCEPT
  {
    if (!listener)
      return;

    if (auto it = std::find(_listeners.begin(), _listeners.end(), listener);
        it != _listeners.end())
    {
      if (_iteratorsNumber) [[unlikely]]
      {
        *it              = nullptr;
        _isCleanupNeeded = true;
      }
      else
      {
        _listeners.erase(it);
      }

      --_listenersNumber;
    }
  }

  template<typename T>
  void TListenerContainer<T>::Cleanup() SRX_NOEXCEPT
  {
    SRX_ASSERT(_iteratorsNumber == 0u);

    if (_isCleanupNeeded)
    {
      _listeners.erase(
        std::remove(_listeners.begin(), _listeners.end(), nullptr),
        _listeners.end());
      _listenersNumber = _listeners.size();
      _isCleanupNeeded = false;
    }
  }

  // Iterator
  template<typename T>
  TListenerContainer<T>::Iterator::Iterator(TListenerContainer* list)
    SRX_NOEXCEPT
    : _list(list)
    , _index(0u)
  {
    Init();
  }

  template<typename T>
  TListenerContainer<T>::Iterator::Iterator(const Iterator& other) SRX_NOEXCEPT
    : _list(other._list)
    , _index(other._index)
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

      _index = other._index;
      _list  = other._list;

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

    return (_index == other._index && _list == other._list);
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
    if (_list)
    {
      _list->_iteratorsNumber++;
      if (_index < _list->_listeners.size() && !_list->_listeners[_index])
        Next();
    }
  }

  template<typename T>
  void TListenerContainer<T>::Iterator::Reset() SRX_NOEXCEPT
  {
    if (_list)
    {
      SRX_ASSERT(_list->_iteratorsNumber > 0);
      _list->_iteratorsNumber--;

      if (_list->_iteratorsNumber == 0u)
        _list->Cleanup();
    }

    _list  = nullptr;
    _index = 0;
  }


  template<typename T>
  void TListenerContainer<T>::Iterator::Next() SRX_NOEXCEPT
  {
    if (_list == nullptr)
      return;

    const size_t size = _list->_listeners.size();
    do
    {
      ++_index;
    } while (_index < size && _list->_listeners[_index] == nullptr);
  }
}
