/**************************************************************************/
/*                         This file is part of:                          */
/*                                SOREX                                   */
/*                 Simple OpenGL Rendering Engine eXtended                */
/**************************************************************************/
/* Copyright (c) 2022-2024 Aleksandr Ershov (Ruffy).                      */
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

#include "CoreMinimal.h"
#include "Containers/ObjectContainer.h"
#include "Containers/ListenerContainer.h"

namespace Sorex
{
  class SRX_API Director
  {
public:
    class Component
    {
      SRX_RTTI_BASE(Director::Component);

  public:
      virtual ~Component() = default;

      virtual void Attach(Director& director);

      virtual Status Initialize() { return SRX_OK; }
      virtual void   Shutdown() {};

      virtual void Update(const float deltaTime) {}

      SRX_INLINE Director* GetDirector() const { return mDirector; }
      SRX_INLINE bool      IsAttached() const { return mDirector != nullptr; }

  private:
      Director* mDirector = nullptr;
    };

    class IListener
    {
  public:
      virtual ~IListener() {}

      /**
       * @brief Will be invoked before start new frame.
       *
       * @param deltaTime - the time difference between the previous frame
       */
      virtual void OnBeginFrame(const float deltaTime) {}

      /**
       * @brief Will be invoked after work to process frame is completed.
       *
       * @param frameTime - elipsis time from the begin frame to finis frame
       */
      virtual void OnFinishFrame(const float frameTime) {}

      /**
       * @brief Will be invoked before the scene update. There must not be
       * any rendering.
       *
       * @param deltaTime - time elapsed since the previous frame
       */
      virtual void OnUpdate(float deltaTime) {}

      /**
       * @brief Will be invoked when frame is being rendered.
       *
       * @param stage - stage of rendering.
       */
      virtual void OnRenderScene() {}

      /**
       * @brief Will be invoked after the main loop exited.
       *
       */
      virtual void OnExit() {}
    };

public:
    static constexpr uint16 kDefaultFrameRate = 60;

public:
    Director() SRX_NOEXCEPT;

    virtual Status Initialize();
    virtual void   Shutdown();

    /**
     * @brief Start main engine loop.
     */
    virtual void Run();

    // Components
    template<typename T, typename... Args>
      requires std::is_base_of_v<Component, T>
    SRX_INLINE T* AddComponent(Args&&... args) SRX_NOEXCEPT;
    Component* AddComponent(TUniquePointer<Component>&& component) SRX_NOEXCEPT;

    template<typename T>
      requires std::is_base_of_v<Component, T>
    SRX_INLINE void RemoveComponent() SRX_NOEXCEPT;
    bool            RemoveComponent(const Component* component) SRX_NOEXCEPT;

    template<typename T>
      requires std::is_base_of_v<Component, T>
    SRX_INLINE const T* GetComponent() const SRX_NOEXCEPT;
    template<typename T>
      requires std::is_base_of_v<Component, T>
    SRX_INLINE T* GetComponent() SRX_NOEXCEPT;

    // Listeners
    SRX_INLINE bool AddListener(IListener* listener) SRX_NOEXCEPT
    {
      return mListeners.Add(listener);
    }

    SRX_INLINE void RemoveListener(IListener* listener) SRX_NOEXCEPT
    {
      mListeners.Remove(listener);
    }

    // FIXME: Add Impl
    virtual void Exit() {}
    virtual bool IsExitRequested() { return false; }
    // virtual int32 GetFrameRate() const          = 0;
    // virtual void  SetFrameRate(int32 frameRate) = 0;

protected:
    virtual Status OnLaunch() { return SRX_OK; }

protected:
    TListenerContainer<IListener> mListeners;

private:
    TObjectContainer<Component> mComponents;

    float  mDeltaTime;
    uint16 mFrameRate;

    bool mIsExitRequested;
  };

  template<typename T, typename... Args>
    requires std::is_base_of_v<Director::Component, T>
  SRX_INLINE T* Director::AddComponent(Args&&... args) SRX_NOEXCEPT
  {
    if (T* const cmp = mComponents.Add<T>(std::forward<Args>(args)...))
    {
      cmp->Attach(*this);
      return cmp;
    }

    return nullptr;
  }

  template<typename T>
    requires std::is_base_of_v<Director::Component, T>
  SRX_INLINE void Director::RemoveComponent() SRX_NOEXCEPT
  {
    if (auto cmp = mComponents.Release<T>())
      cmp->Shutdown();
  }

  template<typename T>
    requires std::is_base_of_v<Director::Component, T>
  SRX_INLINE const T* Director::GetComponent() const SRX_NOEXCEPT
  {
    return mComponents.Get<T>();
  }

  template<typename T>
    requires std::is_base_of_v<Director::Component, T>
  SRX_INLINE T* Director::GetComponent() SRX_NOEXCEPT
  {
    return mComponents.Get<T>();
  }
}  // namespace
