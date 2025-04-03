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

#include <Sorex/SxDirector.h>

#include <Sorex/SxTime.h>
#include <Sorex/SxThread.h>
#include <Sorex/Graphics/SxCanvas.h>

namespace Sorex
{
  Director::Director() SRX_NOEXCEPT
    : mDeltaTime(0.001f)
    , mFrameRate(kDefaultFrameRate)
    , mIsExitRequested(false)
  {}

  Director::~Director()
  {}

  int32 mFrameRate;
  float mDeltaTime;
  bool  mIsExitRequested;

  Status Director::Initialize()
  {
    SRX_CLSFUN_TRACE();

    Graphics::RenderDevice* renderDevice =
      GetComponent<Graphics::RenderDevice>();
    if (renderDevice == nullptr)
      return SRX_STATUS_MSG(EStatusCode::Invalid_State,
                            "[Director] Render device not found");

    Status status;
    for (auto& cmp : mComponents)
    {
      if (cmp == nullptr)
        continue;

      if (status = cmp->Initialize(); !status.Ok())
      {
        SRX_ERROR("[Director] Component '{}' initialization failed: {}",
                  cmp->GetRuntimeClass().GetName(),
                  status.ToString());

        return status;
      }
    }

    mCanvas = MakeUnique<Canvas>(*renderDevice);
    return mCanvas->Initialize();
  }

  void Director::Shutdown()
  {
    SRX_CLSFUN_TRACE();
    for (auto it = mComponents.rbegin(); it != mComponents.rend(); ++it)
    {
      if (Component* const cmp = it->get(); cmp)
        cmp->Shutdown();
    }

    mComponents.Clear();
  }

  void Director::MainLoop()
  {
    SRX_CLSFUN_TRACE();
    if (Status status = OnLaunch(); !status.Ok())
    {
      SRX_ERROR("[Director] Launching failed: {}", status.ToString());
      return;
    }

    int32       millisec;
    uint64      tmNow;
    uint64      tmInterval;
    uint64      tmLast = Time::GetSteadyCounter();
    const int64 tmFrequency =
      static_cast<int64>(Time::GetSteadyCounterFrequency()) - 1ULL;
    const uint64 frameThreshold = mFrameRate ? (tmFrequency / mFrameRate) : 0u;

    while (!mIsExitRequested)
    {
      for (IListener* listener : mListeners)
        listener->OnBeginFrame(mDeltaTime);

      for (auto& cmp : mComponents)
      {
        if (cmp)
          cmp->Update(mDeltaTime);
      }

      OnUpdate(mDeltaTime);

      if (mIsExitRequested)
        break;

      RenderScene();

      // @TODO: call in reverse order
      for (IListener* listener : mListeners)
        listener->OnFinishFrame();

      tmNow      = Time::GetSteadyCounter();
      tmInterval = tmNow - tmLast;
      tmLast     = tmNow;

      if (mFrameRate && tmInterval < frameThreshold)
      {
        const int64 tmWait = frameThreshold - tmInterval;
        millisec           = static_cast<int32>(tmWait * 1000LL / tmFrequency);

        constexpr int64 kSleepTreshold = 1;
        if (millisec > kSleepTreshold)
        {
          Thread::Sleep(millisec);
          tmLast += tmWait;
          tmInterval += tmWait;
        }
      }

      mDeltaTime = static_cast<float>(static_cast<double>(tmInterval * 1000ULL)
                                      / static_cast<double>(tmFrequency));
    }

    for (IListener* listener : mListeners)
      listener->OnExit();
  }

  void Director::RenderScene()
  {
    SRX_CHECK(mCanvas);
    mCanvas->Clear();
    OnDraw(*mCanvas);
    mCanvas->Flush();
  }

  bool Director::RemoveComponent(const Component* component) SRX_NOEXCEPT
  {
    SRX_TRACE(
      "[{}] Remove Component {}",
      this->GetRuntimeClass().GetName(),
      (component ? component->GetRuntimeClass().GetName() : String("null")));

    SRX_CHECK(component && component->GetDirector() == this);

    return mComponents.Remove(component);
  }

  // Component
  void Director::Component::Attach(Director& director)
  {
    SRX_CLSFUN_TRACE();
    SRX_CHECK(!IsAttached());
    mDirector = &director;
  }

  Director::Component* Director::AddComponent(
    TUniquePointer<Component>&& component) SRX_NOEXCEPT
  {
    SRX_CHECK(component && !component->IsAttached());
    if (Component* const cmp = mComponents.Add(std::move(component)))
    {
      SRX_TRACE("[{}] Add Component {}",
                GetRuntimeClass().GetName(),
                cmp->GetRuntimeClass().GetName());

      cmp->Attach(*this);
      return cmp;
    }

    return nullptr;
  }
}  // namespace
