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

#include <Sorex/Asset/SxTextureLoader.h>

#include <Sorex/FileSystem/SxPathUtils.h>
#include <Asset/SxTgaImageLoader.h>
#include <Asset/SxDefaultImageLoader.h>

namespace
{
  using namespace Sorex;
  template<typename T>
  class ImageLoaderCreator final: public TObjectCreator<Resource::ImageLoader>
  {
    static_assert(std::is_base_of_v<Resource::ImageLoader, T>, "invalid type");

public:
    virtual TUniquePointer<Resource::ImageLoader> Create() const override
    {
      return MakeUnique<T>();
    };
  };
}  // namespace

namespace Sorex::Resource
{
  TextureCreator::TextureCreator(Graphics::RenderDevice& renderDevice,
                                 const bool bEnableDefaultImageLoader)
    : mRenderDevice(renderDevice)
    , mEnableDefaultImageLoader(bEnableDefaultImageLoader)
  {}

  void TextureCreator::RegisterImageLoader(
    const String&                               extenstion,
    TUniquePointer<TObjectCreator<ImageLoader>> creator)
  {
    UniqueLock _lock(mMutex);
    mImgLoadersFactory.RegisterCreator(extenstion, std::move(creator));
  }

  TUniquePointer<ImageLoader> TextureCreator::CreateImageLoader(
    const String& key) const
  {
    SharedLock _lock(mMutex);  // TODO: Do we need mutex at this class?
    if (auto loader = mImgLoadersFactory.Create(key))
      return loader;

    // @TODO: Maybe it makes sense to set it only for known extensions: PNG,
    // TGA, JPEG
    return mEnableDefaultImageLoader ? MakeUnique<DefaultImageLoader>()
                                     : nullptr;
  }

  AssetCreator::AssetInstance TextureCreator::CreateAssetInstance(
    Path           path,
    AssetRegistry* registry,
    Status*        status)
  {
    TSharedPointer<Graphics::Texture2D> asset =
      mRenderDevice.CreateTexture2D(std::move(path));
    if (asset && registry)
    {
      if (auto st = registry->Register(asset); !st.Ok())
      {
        if (status)
          *status = std::move(st);
        return std::make_pair(nullptr, nullptr);
      }
    }

    auto loader = MakeUnique<TextureLoader>(*this, asset);
    return std::make_pair(std::move(asset), std::move(loader));
  }

  TextureLoader::TextureLoader(const TextureCreator&               creator,
                               TSharedPointer<Graphics::Texture2D> texture)
    : AssetLoader(std::move(texture))
    , mCreator(creator)
  {}

  Status TextureLoader::Preload(AssetStorage&  storage,
                                AssetRegistry* registry,
                                TVector<Path>& missingFiles)
  {
    const auto& path = GetAssetPath();
    SRX_DEBUG("[TextureLoader] Preload {} asset '{}'",
              GetTypeName<Graphics::Texture2D>(),
              GetAssetName());

    const String extansion = path.extension().generic_string();
    if (extansion.empty())
    {
      // TODO: See RE to implement
      return SRX_STATUS(EStatusCode::Not_Implemented);
    }

    if (!storage.Contains(path.native()))
    {
      missingFiles.push_back(path);
      return SRX_OK;
    }

    mLoader = mCreator.CreateImageLoader(extansion.substr(1));
    if (!mLoader)
      return SRX_STATUS_MSG(
        EStatusCode::Not_Found,
        "image data loader for '{}' key not found, asset: '{}'",
        extansion,
        GetAssetName());
    return SRX_OK;
  }

  Status TextureLoader::Load(AssetStorage&       storage,
                             const AssetOptions* options,
                             AssetDependencies&  dependencies)
  {
    SRX_CHECK(mLoader);
    SRX_DEBUG("[TextureLoader] Load {} asset '{}'",
              GetTypeName<Graphics::Texture2D>(),
              GetAssetName());

    Status                 status;
    TUniquePointer<Stream> stream = storage.Read(GetAssetPath(), &status);
    if (stream == nullptr)
      return status;

    mBitmap = mLoader->LoadImage(*stream, &status);
    mLoader.reset();

    return status;

    /*   const auto format = mCreator.GetRenderDevice().GetSupportedPixelFormat(
        _bitmap->GetPixelFormat());
      if (format != _bitmap->GetPixelFormat())
      {
        if (!_bitmap->ConvertToFormat(format, error))
          return false;
      }
   */
  }

  Status TextureLoader::Finalize(AssetRegistry*           registry,
                                 const AssetDependencies& dependencies)
  {
    SRX_CHECK(mBitmap && GetAssetPtr());
    SRX_DEBUG("[TextureLoader] Finalize loading {} asset '{}'",
              GetTypeName<Graphics::Texture2D>(),
              GetAssetName());

    Graphics::Texture2D* texture =
      static_cast<Graphics::Texture2D*>(GetAssetPtr());
    return texture->Initialize(std::move(mBitmap));
  }
}  // namespace
