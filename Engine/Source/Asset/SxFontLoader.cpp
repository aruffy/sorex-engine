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

#include <Sorex/Asset/SxFontLoader.h>
#include <Sorex/Graphics/SxTexture2D.h>

#include <Asset/Font/SxXmlFontDataLoader.h>
#include <Utils/XmlReader.h>

namespace Sorex::Resource
{
  FontCreator::FontCreator(Graphics::RenderDevice& renderDevice)
    : mRenderDevice(renderDevice)
  {
    mFactory.RegisterCreator<
      DefaultObjectCreator<XMLFontDataLoader, IFontDataLoader>>("xml");
    /* mFactory.RegisterCreator<Details::TrueTypeFontLoaderCreator>("ttf"); */
  }

  void FontCreator::RegisterFontLoader(
    const String&                                   extenstion,
    TUniquePointer<TObjectCreator<IFontDataLoader>> creator)
  {
    UniqueLock _lock(mMutex);
    mFactory.RegisterCreator(extenstion, std::move(creator));
  }

  TUniquePointer<IFontDataLoader> FontCreator::CreateFontDataLoader(
    const String& name) const
  {
    SharedLock _lock(mMutex);  // TODO: Do we need mutex at this class?
    return mFactory.Create(name);
  }

  AssetCreator::AssetInstance FontCreator::CreateAssetInstance(
    Path           path,
    AssetRegistry* registry,
    Status*        status)
  {
    TSharedPointer<Graphics::Font> asset =
      MakeShared<Graphics::Font>(std::move(path));

    if (asset && registry)
    {
      if (auto st = registry->Register(asset); !st.Ok())
      {
        if (status)
          *status = std::move(st);

        return std::make_pair(nullptr, nullptr);
      }
    }

    auto loader = MakeUnique<FontLoader>(*this, asset);
    return std::make_pair(std::move(asset), std::move(loader));
  }

  FontLoader::FontLoader(const FontCreator&             creator,
                         TSharedPointer<Graphics::Font> font)
    : AssetLoader(std::move(font))
    , mCreator(creator)
    , mFontBitmapId(-1)
  {}

  Status FontLoader::Preload(AssetStorage&  storage,
                             AssetRegistry* registry,
                             TVector<Path>& missingFiles)
  {
    const auto& path = GetAssetPath();
    if (path.empty())
      return SRX_STATUS_MSG(EStatusCode::Invalid_Argument, "empty asset path");

    SRX_DEBUG("[FontLoader] Preload {} asset '{}'",
              GetTypeName<Graphics::Font>(),
              GetAssetName());

    const String extansion = path.extension().generic_string();
    if (extansion.empty())
    {
      // TODO: See RE to implement
      return SRX_STATUS(EStatusCode::Not_Implemented);
    }

    mLoader = mCreator.CreateFontDataLoader(extansion.substr(1));
    if (!mLoader)
      return SRX_STATUS_MSG(
        EStatusCode::Not_Found,
        "font data loader for '{}' key not found, asset: '{}'",
        extansion,
        GetAssetName());

    return SRX_OK;
  }

  Status FontLoader::Load(AssetStorage&       storage,
                          const AssetOptions* options,
                          AssetDependencies&  dependencies)
  {
    SRX_ASSERT(mLoader);
    SRX_DEBUG("[FontLoader] Load {} asset '{}'",
              GetTypeName<Graphics::Font>(),
              GetAssetName());

    Status status;
    auto   stream = storage.Read(GetAssetPath(), &status);
    if (stream == nullptr)
      return status;

    auto [fontData, bitmap] = mLoader->LoadFont(*stream, &status);
    if (!fontData)
      return status;

    if (!bitmap)
    {
      Path bitmapPath = GetAssetPath();
      bitmapPath.replace_extension("png");  // @FIXME: hardcoded extension

      mFontBitmapId =
        dependencies.Push<Graphics::Texture2D>(std::move(bitmapPath));
      SRX_CHECK(mFontBitmapId >= 0);  // NOTE: Push should always succeed
    }

    mFontData = std::move(fontData);
    mBitmap   = std::move(bitmap);

    return SRX_OK;
  }

  Status FontLoader::Finalize(AssetRegistry*           registry,
                              const AssetDependencies& dependencies)
  {
    SRX_CHECK(mFontData);
    SRX_DEBUG("[FontLoader] Finalize loading {} asset '{}'",
              GetTypeName<Graphics::Font>(),
              GetAssetName());

    const String&                       name = GetAssetPtr()->GetName();
    TSharedPointer<Graphics::Texture2D> texture;
    if (mBitmap)
    {
      texture = mCreator.GetRenderDevice().CreateTexture2D(Path(name));
      if (texture)
      {
        if (Status s = texture->Initialize(std::move(mBitmap)); !s.Ok())
          return s;
      }
    }
    else
    {
      texture = dependencies.GetAsset<Graphics::Texture2D>(mFontBitmapId);
    }

    if (!texture)
      return SRX_STATUS_MSG(EStatusCode::No_Data,
                            "missing {} dependecy '{}'",
                            GetTypeName<Graphics::Texture2D>(),
                            name);

    Graphics::Font* font = static_cast<Graphics::Font*>(GetAssetPtr());
    return font->Initialize(std::move(mFontData), std::move(texture));
  }
}  // namespace Sorex::Resource