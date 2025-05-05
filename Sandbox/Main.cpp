#include <iostream>

#include <Sorex/SxRuntimeClass.h>
#include <Sorex/SxStatus.h>
#include <Sorex/SxJournalManager.h>

#include <Sorex/SxTime.h>
#include <Sorex/SxLauncher.h>
#include <Sorex/SxDirector.h>
#include <Sorex/FileSystem/SxDirectorFileSystem.h>
#include <Sorex/Graphics/SxCanvas.h>

#include <Sorex/Asset/SxAsset.h>
#include <Sorex/Asset/SxAssetManager.h>
#include <Sorex/Asset/SxAssetFileSystemStorage.h>
#include <Sorex/Asset/SxTextureLoader.h>
#include <Sorex/FileSystem/SxPathUtils.h>

#include <Sorex/SxDesktopLauncher.h>

using namespace Sorex;

class MyDirector final: public Director
{
  public:
  virtual Status Initialize() override
  {
    auto filesystem = AddComponent<DirectorFileSystem>();
    SRX_ASSERT(filesystem);

    mAssetStorage = MakeUnique<Resource::AssetFileSystemStorage>(*filesystem);
    mAssetManager =
      AddComponent<Resource::AssetManager>(*mAssetStorage, nullptr);

    Status status = Director::Initialize();
    if (!status.Ok())
      return status;

    auto glRenderDevice = GetComponent<Graphics::RenderDevice>();
    SRX_ASSERT(glRenderDevice);

    mAssetManager->Register<Graphics::Texture2D>(
      MakeUnique<Resource::TextureCreator>(*glRenderDevice));

    status = filesystem->Mount(SRX_PATH("Textures"), SRX_PATH("/Textures"));
    SRX_ASSERT(status.Ok());

    /*     auto asset2 = mAssetManager->LoadAsync<Graphics::Texture2D>(
          SRX_PATH("/Textures/image2.tga"),
          nullptr,
          nullptr);

        mTexture =
          mAssetManager->Load<Graphics::Texture2D>(SRX_PATH("/Textures/image.tga"),
                                                   nullptr,
                                                   nullptr); */
    return status;
  }

  protected:
  virtual void OnDraw(Canvas& canvas) override
  {
    static float f = 0.f;
    f += 5.f;
    if (f >= 400.f)
      f = 0.f;

    canvas.DrawLine(Point(f, 200.f), Point(f + 200.f, 200.f), Color::Purple);
    canvas.DrawRect(Rect(Point(f, 325.f), Size(64.f, 64.f)), Color::Green);
    canvas.DrawRect(Rect(Point(200.f, 400.f), Size(64.f, 128.f)),
                    Color::Yellow);
    canvas.DrawCircle(Point(300.f, 400.f), 64.f, 32, Color::Red);

    /* if (mTexture)
      canvas.DrawTexture(mTexture.get(), Point(200.f, 25.f)); */
  }

  private:
  TUniquePointer<Resource::AssetStorage> mAssetStorage;
  SxAssetManager*                        mAssetManager = nullptr;
  TSharedPointer<Graphics::Texture2D>    mTexture;
};

int main(const int argc, const char* argv[])
{
  if (!(JournalManager::GetInstance()
          .RegisterLogger<JournalManager::kEngineLogger>("SorexEngine", true)
          .Ok()))
    return 1;

  Platform::DesktopLauncher().Run<MyDirector>();

  return 0;
}
