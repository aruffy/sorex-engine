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

#include <Sorex/Director.h>

namespace Sorex
{
  Status Director::Initialize()
  {
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

    return status;
  }

  void Director::Shutdown()
  {
    for (auto& cmp : mComponents)
    {
      if (cmp)
        cmp->Shutdown();
    }

    mComponents.Clear();
  }

  void Director::Component::Attach(Director& director)
  {
    SRX_CHECK(!IsAttached());
    mDirector = &director;
  }

  Director::Component* Director::AddComponent(
    TUniquePointer<Component>&& component) SRX_NOEXCEPT
  {
    SRX_CHECK(component && !component->IsAttached());
    if (Component* const cmp = mComponents.Add(std::move(component)))
    {
      cmp->Attach(*this);
      return cmp;
    }

    return nullptr;
  }

  bool Director::RemoveComponent(const Component* component) SRX_NOEXCEPT
  {
    SRX_CHECK(component && component->GetDirector() == this);
    return mComponents.Remove(component);
  }
}  // namespace
