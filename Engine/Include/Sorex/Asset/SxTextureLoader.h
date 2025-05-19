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
#include <Sorex/Graphics/SxTexture2D.h>
#include <Sorex/Utils/SxObjectFactory.h>

#include "SxAssetCreator.h"
#include "SxAssetLoader.h"

namespace Sorex::Resource
{
  class ImageLoader
  {
public:
    virtual ~ImageLoader() = default;

    /**
     * @brief Load image file from stream to texture bitmap.
     *
     * @note: can be invoked from different threads
     *
     * @param stream - source of image file
     * @param status [out] - status description
     *
     * @return pointer to texture bitmap or Null if error occured.
     */
    virtual TUniquePointer<Graphics::TextureBitmap> LoadImage(
      Stream& stream,
      Status* status) = 0;
  };

  class TextureCreator final: public AssetCreator
  {
public:
    explicit TextureCreator(Graphics::RenderDevice& renderDevice,
                            const bool bEnableDefaultImageLoaders = true);

    virtual AssetInstance CreateAssetInstance(Path           path,
                                              AssetRegistry* registry,
                                              Status*        status) override;

    void RegisterImageLoader(
      const String&                               extenstion,
      TUniquePointer<TObjectCreator<ImageLoader>> creator);

    TUniquePointer<ImageLoader> CreateImageLoader(const String& key) const;

private:
    mutable ShMutex mMutex;

    Graphics::RenderDevice&     mRenderDevice;
    TObjectFactory<ImageLoader> mImgLoadersFactory;

    bool mEnableDefaultImageLoader;
  };

  class TextureLoader final: public AssetLoader
  {
public:
    TextureLoader(const TextureCreator&               creator,
                  TSharedPointer<Graphics::Texture2D> texture);

    virtual Status Preload(AssetStorage&  storage,
                           AssetRegistry* registry,
                           TVector<Path>& missingFiles) override;

    virtual Status Load(AssetStorage&       storage,
                        const AssetOptions* options,
                        AssetDependencies&  dependencies) override;

    virtual Status Finalize(AssetRegistry*           registry,
                            const AssetDependencies& dependencies) override;

private:
    const TextureCreator&                   mCreator;
    TUniquePointer<ImageLoader>             mLoader;
    TUniquePointer<Graphics::TextureBitmap> mBitmap;
  };
}  // namespace
