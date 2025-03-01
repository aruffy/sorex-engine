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
#include <Sorex/Asset/SxAsset.h>

#include "SxTextureBitmap.h"

namespace Sorex::Graphics
{
  class Texture2D: public Sorex::Resource::Asset
  {
    SRX_RTTI(Graphics::Texture2D, Resource::Asset);

public:
    explicit Texture2D(StringView name);
    // virtual ~Texture2D() override = default;

    /**
     * @brief Initialize texture with certain bitmap.
     *
     * @param bitmap - texture bitmap
     * @param error - error description
     * @return - True if initialization was successful, else False.
     */
    // virtual bool Initialize(const TextureBitmap* bitmap, Error* error) = 0;

    /**
     * @brief Initialize texture with certain bitmap.
     *
     * @param bitmap - texture bitmap
     * @param error - error description
     * @return - True if initialization was successful, else False.
     */
    virtual Status Initialize(TUniquePointer<TextureBitmap> bitmap) = 0;

    /**
     * @brief Retrieve scale of texture.
     *
     * @return value of texture scale.
     */
    virtual float GetScale() const { return 1.f; }

    /**
     * @brief Retrieve size of texture.
     *
     * Width and height values of size are always power of two.
     *
     * @return size of texture;
     */
    virtual SizeInt GetSize() const = 0;

    /**
     * @brief Retrive size of raw texture.
     *
     * Raw texture (TextureBitmap) can have any size, but the size of texture
     * class could be expanded to power of two. This method must return real
     * location and size of the raw texture content.
     *
     * @return rectangle of the texture;
     */
    virtual Rectangle GetContentRect() const = 0;

    /**
     * @brief Retrieve texture format.
     *
     * @return format of the texture.
     */
    virtual ETextureFormat GetFormat() const = 0;

    /**
     * @brief Retrieve texture sampler.
     *
     */
    // virtual const TextureSampler* GetSampler() const = 0;

    /**
     * @brief Set texture sampler.
     *
     */
    // virtual void SetSampler(const TextureSampler* sampler) = 0;
  };
}  // namespace

using SxTexture = Sorex::Graphics::Texture2D;
