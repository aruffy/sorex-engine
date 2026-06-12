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

#include <Sorex/SxDirector.h>
#include <Sorex/SxStream.h>
#include <Sorex/FileSystem/SxFileSystem.h>

#include "SxRenderer.h"
#include "SxTextureBitmap.h"

namespace Sorex::Graphics
{
  class Texture2D;
  class RenderDevice: public Sorex::Director::Component
  {
    SRX_RTTI(Graphics::RenderDevice, Director::Component)

public:
    RenderDevice()                   = default;
    virtual ~RenderDevice() override = default;

    virtual void Cleanup() = 0;

    template<typename T>
      requires std::is_base_of_v<Renderer, T>
    TUniquePointer<T> CreateRenderer(const ssize_t capacity = SRX_UNKNOWN_SIZE)
      SRX_NOEXCEPT
    {
      if (auto renderer = CreateRenderer(GetRuntimeType<T>(), capacity))
      {
        SRX_CHECK_MSG(renderer->template IsA<T>(), "invalid renderer type");
        return TUniquePointer<T>(static_cast<T*>(renderer));
      }

      return nullptr;
    }

    /**
     * @brief Create new 2D texture that can be handled by the render device.
     *
     * @param name - name of the texture
     * @return pointer to 2D texture;
     */
    virtual TUniquePointer<Texture2D> CreateTexture2D(Path path) = 0;

    /**
     * @brief Retrieve supported texture pixel format.
     *
     * If the arg `format` is supported that it must be the result;
     * Else should find similar supported pixel format for the `format` from
     * args list; If it is inpossible, return pixel format that has bigger color
     * component size (no need to compress color). For example: ARGB1555 ->
     * RGBA5551.
     *
     * @param - source format;
     * @return - supported pixel format.
     */
    // virtual EPixelFormat GetSupportedPixelFormat(EPixelFormat format) const =
    // 0;

protected:
    virtual Renderer* CreateRenderer(const RuntimeClass& cls,
                                     ssize_t capacity) SRX_NOEXCEPT = 0;
  };

  class IRenderDeviceResource
  {
public:
    virtual RenderDevice* GetRenderDevice() const = 0;

protected:
    virtual ~IRenderDeviceResource() = default;
  };
}  // namespace
