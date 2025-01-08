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

#include <Sorex/Graphics/SxRenderDevice.h>

#include "GLTypes.h"
#include "GLResourceToken.h"

namespace Sorex::Graphics
{
  class GLRenderDevice final: public RenderDevice
  {
    SRX_RTTI(Graphics::GLRenderDevice, Graphics::RenderDevice)

public:
    // Interface Director::Component
    virtual Status Initialize() override;

    /**
     * @brief Allocate new OpenGL resource.
     *
     * @param type - type of resource
     * @return resource token
     */
    GLResourceToken Allocate(GLResourceType type) SRX_NOEXCEPT;

    /**
     * @brief Deallocate OpenGL resource.
     *
     * @param glResource - resource reference
     */
    void Deallocate(GLResourceReference* glResource) SRX_NOEXCEPT;

protected:
    virtual TUniquePointer<Renderer> CreateRenderer(const RuntimeClass& cls,
                                                    ssize_t capacity)
      SRX_NOEXCEPT override
    {
      return nullptr;
    }

private:
    TList<GLResource> mResources;
  };

}  // namespace
