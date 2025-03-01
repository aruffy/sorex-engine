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

namespace Sorex::Resource
{
  TextureCreator::TextureCreator(Graphics::RenderDevice& renderDevice,
                                 const bool bEnableDefaultImageLoader)
    : mRenderDevice(renderDevice)
  {
    if (bEnableDefaultImageLoader)
    {
      // Register Default Image Loaders
    }
  }

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
    return mImgLoadersFactory.Create(key);
  }

  TSharedPointer<Asset> TextureCreator::CreateAssetInstance(
    StringView     name,
    AssetRegistry* registry,
    Status*        status)
  {
    TSharedPointer<Graphics::Texture2D> asset =
      mRenderDevice.CreateTexture2D(name);
    if (asset && registry)
    {
      // const Registry::ECache cache =
      //   options ? options->GetAssetRegistryCache<Graphics::Texture2D>()
      //           : Registry::ECache::Default;
      //
      if (auto st = registry->Register(asset); !st.Ok())
      {
        if (status)
          *status = std::move(st);
        return nullptr;
      }
    }

    return asset;
  }

  TUniquePointer<AssetLoader> TextureCreator::CreateAssetLoader(
    const TSharedPointer<Asset>& asset,
    Status*                      status)
  {
    if (!IsLoadableReference<Graphics::Texture2D>(asset.get()))
    {
      if (status)
        *status = SRX_STATUS_MSG(EStatusCode::Invalid_Argument,
                                 "invalid asset reference");
      return nullptr;
    }

    return MakeUnique<TextureLoader>(
      *this,
      std::static_pointer_cast<Graphics::Texture2D>(asset));
  }

  TextureLoader::TextureLoader(const TextureCreator&               creator,
                               TSharedPointer<Graphics::Texture2D> texture)
    : AssetLoader(std::move(texture))
  // , mCreator(creator)
  {}

  Status TextureLoader::Preload(AssetStorage&    storage,
                                AssetRegistry*   registry,
                                TVector<String>& missingFiles)
  {
    const String& name = GetAssetName();
    SRX_DEBUG("Preload {} asset '{}'",
              GetTypeName<Graphics::Texture2D>(),
              name);
    /*
       String extansion = String(PathUtils::GetFileExtension(name));
       if (extansion.empty())
       {
         TVector<String> images;
         storage.GetAll(name,
                        images);  // @todo: it's not efficient (asset metadata?)

         if (images.empty())
         { missingFiles.push_back(name);
           return true;
         }

         if (images.size() != 1)
         {
           RFY_MAKE_ERR(error,
                        Error::Not_Unique,
                        "texture loading files <{}> conflict",
                        images.size());
           return false;
         }

         _path     = std::move(images.front());
         extansion = String(PathUtils::GetFileExtension(_path));
       }
       else
       {
         if (!storage.Contains(name))
         {
           missingFiles.push_back(name);
           return true;
         }

         _path = name;
       }

       _loader = _creator.CreateImageLoader(extansion);
       if (!_loader)
       {
         RFY_MAKE_ERR(error,
                      Error::Not_Found,
                      "image data loader for '{}' not found",
                      _path);
         return false;
       }
     */

    return SRX_STATUS(EStatusCode::Not_Implemented);
  }

  Status TextureLoader::Load(AssetStorage&       storage,
                             const AssetOptions* options,
                             AssetDependencies&  dependencies)
  {
    // SRX_CHECK(mLoader && !mPath.empty());
    SRX_DEBUG("Load {} asset '{}'",
              GetTypeName<Graphics::Texture2D>(),
              GetAssetName());
    /*
       TUniquePointer<Stream> stream = storage.Read(_path, error);
       if (stream == nullptr)
         return false;

       _bitmap = _loader->Load(stream.get(), error);
       if (!_bitmap)
         return false;

       const auto format = _creator.GetRenderDevice().GetSupportedPixelFormat(
         _bitmap->GetPixelFormat());
       if (format != _bitmap->GetPixelFormat())
       {
         if (!_bitmap->ConvertToFormat(format, error))
           return false;
       }

       return true;
     */

    return SRX_STATUS(EStatusCode::Not_Implemented);
  }

  Status TextureLoader::Finalize(AssetRegistry*           registry,
                                 const AssetDependencies& dependencies)
  {
    SRX_DEBUG("Finalize loading {} asset '{}'",
              GetTypeName<Graphics::Texture2D>(),
              GetAssetName());

    // Graphics::Texture2D* texture =
    //   static_cast<Graphics::Texture2D*>(GetAssetPtr());
    /*
      if (!texture->Initialize(std::move(_bitmap), error))
        return false;

      return true;
  **/
    return SRX_STATUS(EStatusCode::Not_Implemented);
  }
}  // namespace
