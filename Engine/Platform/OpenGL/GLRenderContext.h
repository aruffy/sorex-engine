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
// #include <Ruffy/Graphics/Rendering/BlendMode.h>
// #include <Ruffy/Graphics/Rendering/TextureSampler.h>

#include "GLTypes.h"
#include "GLRenderTechnique.h"
#include "GLTexture2D.h"

namespace Sorex::Graphics
{
  class GLRenderDevice;
  class GLRenderContext
  {
public:
    struct Blend
    {
      // BlendMode mode;
      // Color     color;
    };

public:
    explicit GLRenderContext(const GLRenderDevice& renderDevice) SRX_NOEXCEPT;

    void Apply(const GLRenderTechnique& technique) SRX_NOEXCEPT;

    void Reset() SRX_NOEXCEPT;
    void Clear() SRX_NOEXCEPT;  // cppcheck-suppress functionStatic

    void SetColor(const Color value) { mColor = value; }

    Status SetTexture(size_t slot, const GLTexture2D& texture);
    // error_t SetTextureSampler(size_t slot, const TextureSampler& sampler);

    Status ActivateTexture(GLenum slot);

private:
    // void ApplyBlendMode(BlendMode mode);
    /* void ApplyTextureSampler(GLenum                target,
                             const TextureSampler& sampler,
                             bool                  bMipmaps = false);
*/
private:
    const GLRenderDevice& mDevice;

    Color mColor;
    // Blend mBlend;

    struct TextureSample
    {
      const GLTexture2D* texture = nullptr;

      // TextureSampler sampler;
      // bool bUpdateSampler = true;
    };

    TVector<TextureSample> mTextures;
  };
}
