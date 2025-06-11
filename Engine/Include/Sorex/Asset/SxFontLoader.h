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
#include <Sorex/Graphics/SxFont.h>
#include <Sorex/Graphics/SxRenderDevice.h>
#include <Sorex/Utils/SxObjectFactory.h>

#include <Sorex/Asset/SxAssetCreator.h>
#include <Sorex/Asset/SxAssetLoader.h>

namespace Sorex::Resource
{
  class IFontDataLoader
  {
public:
    using glyph_t = Graphics::glyph_t;

public:
    virtual ~IFontDataLoader() {}

    virtual TPair<TUniquePointer<Graphics::FontData>,
                  TUniquePointer<Graphics::TextureBitmap>>
    Load(Stream& stream, Status* status) = 0;
  };

  class FontCreator final: public AssetCreator
  {
public:
    explicit FontCreator(Graphics::RenderDevice& renderDevice);

    FontCreator(const FontCreator& other)            = delete;
    FontCreator& operator=(const FontCreator& other) = delete;

    /**
     * @copydoc AssetCreator::CreateAssetInstance
     */
    virtual AssetInstance CreateAssetInstance(Path           path,
                                              AssetRegistry* registry,
                                              Status*        status) override;

    void RegisterImageLoader(
      const String&                                   extenstion,
      TUniquePointer<TObjectCreator<IFontDataLoader>> creator);

    TUniquePointer<IFontDataLoader> CreateFontDataLoader(
      const String& name) const;

    Graphics::RenderDevice& GetRenderDevice() const { return mRenderDevice; }

private:
    mutable ShMutex mMutex;

    Graphics::RenderDevice&         mRenderDevice;
    TObjectFactory<IFontDataLoader> mFactory;
  };

  class FontLoader final: public AssetLoader
  {
public:
    FontLoader(const FontCreator& creator, TSharedPointer<Graphics::Font> font);

    virtual Status Preload(AssetStorage&  storage,
                           AssetRegistry* registry,
                           TVector<Path>& missingFiles) override;

    virtual Status Load(AssetStorage&       storage,
                        const AssetOptions* options,
                        AssetDependencies&  dependencies) override;

    virtual Status Finalize(AssetRegistry*           registry,
                            const AssetDependencies& dependencies) override;

private:
    const FontCreator& mCreator;

    TUniquePointer<IFontDataLoader> mLoader;

    TUniquePointer<Graphics::FontData>      mFontData;
    TUniquePointer<Graphics::TextureBitmap> mBitmap;
  };
}