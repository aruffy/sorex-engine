#include <iostream>

#include <Sorex/SxRuntimeClass.h>
#include <Sorex/SxStatus.h>
#include <Sorex/SxJournalManager.h>

#include <Sorex/SxTime.h>
#include <Sorex/SxLauncher.h>
#include <Sorex/SxDirector.h>
#include <Sorex/FileSystem/SxDirectorFileSystem.h>
#include <Sorex/Graphics/SxCanvas.h>
#include <Sorex/Graphics/SxFont.h>
#include <Sorex/Graphics/SxFontDecorator.h>

#include <Sorex/Asset/SxAsset.h>
#include <Sorex/Asset/SxAssetManager.h>
#include <Sorex/Asset/SxAssetFileSystemStorage.h>
#include <Sorex/FileSystem/SxPathUtils.h>

#include <Sorex/Asset/SxTextureLoader.h>
#include <Sorex/Asset/SxFontLoader.h>

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

    auto renderDevice = GetComponent<Graphics::RenderDevice>();
    SRX_ASSERT(renderDevice);

    mAssetManager->Register<Graphics::Texture2D>(
      MakeUnique<Resource::TextureCreator>(*renderDevice));
    mAssetManager->Register<Graphics::Font>(
      MakeUnique<Resource::FontCreator>(*renderDevice));

    status = filesystem->Mount(SRX_PATH("Textures"), SRX_PATH("/Textures"));
    SRX_ASSERT(status.Ok());
    status = filesystem->Mount(SRX_PATH("Fonts"), SRX_PATH("/Fonts"));
    SRX_ASSERT(status.Ok());

    /* auto asset2 = mAssetManager->LoadAsync<Graphics::Texture2D>(
      SRX_PATH("/Textures/image2.tga"),
      nullptr,
      nullptr); */

    mTexture = mAssetManager->Load<Graphics::Texture2D>(
      SRX_PATH("/Textures/awesomeface.png"),
      nullptr,
      nullptr);

    mFont = mAssetManager->LoadAsync<Graphics::Font>(
      SRX_PATH("/Fonts/Arial-Bold.xml"));
    mSdfFont = mAssetManager->LoadAsync<Graphics::Font>(
      SRX_PATH("/Fonts/SDF-Font-Test.xml"));
    return status;
  }

  protected:
  virtual void OnDraw(Canvas& canvas) override
  {
    // DrawPrimitives(canvas);
    // DrawTextures(canvas);

    // canvas.DrawTexture(mTexture.get(), Point(10.f, 10.f));
    DrawText(canvas);

    /*     canvas.PushPencil();
        Graphics::TextureSampler sampler(
          Graphics::ETextureWrapping::Mirrored_Repeat,
          Graphics::ETextureFilter::Nearest);

        sampler.SetTexCoords(Vec2(2.f, 2.f));
        canvas.SetTextureSampler(sampler);
        canvas.SetBlendMode(Graphics::BlendMode::Alpha);

        canvas.DrawTexture(mTexture.get(), Point(5.f, 5.f));
        canvas.PopPencil(); */
  }

  private:
  void DrawPrimitives(Canvas& canvas);
  void DrawTextures(Canvas& canvas);
  void DrawText(Canvas& canvase);

  private:
  TUniquePointer<Resource::AssetStorage> mAssetStorage;
  SxAssetManager*                        mAssetManager = nullptr;
  TSharedPointer<Graphics::Texture2D>    mTexture;
  TSharedPointer<Graphics::Font>         mFont;
  TSharedPointer<Graphics::Font>         mSdfFont;
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

void MyDirector::DrawPrimitives(Canvas& canvas)
{
  static float f = 0.f;
  f += 5.f;
  if (f >= 400.f)
    f = 0.f;

  canvas.DrawLine(Point(f, 200.f), Point(f + 200.f, 200.f), Color::Purple);
  canvas.DrawRect(Rect(Point(f, 325.f), Size(64.f, 64.f)), Color::Green);
  canvas.DrawRect(Rect(Point(200.f, 400.f), Size(64.f, 128.f)), Color::Yellow);

  canvas.DrawCircle(Point(300.f, 400.f), 64.f, 32, Color::Red);
}

void MyDirector::DrawTextures(Canvas& canvas)
{
  if (!mTexture || !mTexture->IsReady())
    return;

  // 1. scaled texture
  canvas.DrawTexture(mTexture.get(),
                     Point(5.f, 5.f),
                     scalar_t(0),
                     Vec2(0.5f, 0.5f));

  // 2. Blending
  canvas.PushPencil();
  canvas.SetBlendMode(Graphics::BlendMode::Alpha);
  canvas.DrawTexture(mTexture.get(), Point(200.f, 25.f));

  // 3. rotated, blended, colored, scaled
  static float s_angle = 0.f;
  if (s_angle >= 360.f)
    s_angle = 0.f;
  else
    s_angle += 1.f;

  canvas.DrawTexture(mTexture.get(),
                     Point(50.f, 250.f),
                     s_angle * 2.f,
                     Vec2(0.25f, 0.25f),
                     Color(Color::Purple, 128));

  canvas.PopPencil();
}

void MyDirector::DrawText(Canvas& canvas)
{
  if (!mFont || !mFont->IsReady())
    return;

  canvas.PushPencil();
  canvas.SetBlendMode(Graphics::BlendMode::Alpha);
  static float alpha = 0.f;
  alpha += 0.005f;
  if (alpha > 1.f)
    alpha = 0.f;

  scalar_t ypos = 40.f - mFont->GetSize();
  canvas.DrawRect(Rect(Point(10.f, ypos), Size(590.f, mFont->GetSize() + 2.f)),
                  Color::Blue);
  canvas.DrawText(*mFont,
                  "0123 456.001, 321.11987",
                  Point(15.f, 40.f),
                  1.f,
                  Color(Color::Yellow, 1.f));
  canvas.PopPencil();

  if (mSdfFont && mSdfFont->IsReady())
  {
    canvas.PushPencil();
    canvas.SetBlendMode(Graphics::BlendMode::Alpha);
    Color      _color;
    const auto time = Time::GetMonotonicCounter();
    _color.r = _color.g = static_cast<uint8>(time / 10 % 255);
    _color.b            = static_cast<uint8>(time / 50 % 255);
    const int64 sec     = time / 100u;
    const int   t       = sec % 300;
    float       scale   = 0.5f + (float(t) / 100);

    Graphics::FontDecorator decorator;
    decorator.SetScale(scale)
      .SetColor(Color(200, 200, 200, 96))
      .SetLetterSpacingUnit(0.15f)
      .SetOutline(2, Color::Red)
      // .SetTextTransform(Graphics::EFontTransform::Lowercase)
      .Apply(mSdfFont.get());

    Graphics::FontMetrics fontMetrics;
    const auto            leading = decorator.CalcFontMetrics(fontMetrics)
                                      ? fontMetrics.leading
                                      : mSdfFont->GetMetrics()->leading;

    Point      loc(10.f, 120.f);
    const Size boxSize  = Size(780.f, leading);
    Color      boxColor = Color::Purple;
    canvas.DrawRect(Rect(loc, boxSize), boxColor);
    canvas.DrawText(decorator, "A", loc);
    /*
        loc.y += leading;
        canvas.DrawText(decorator, "abcdifghijklmABC", loc);

        loc.y += leading;
        decorator.SetSize(48).SetColor(Color::Green).SetOutline(2, Color::Blue);
        // canvas.DrawRectangle(Rect(loc, boxSize), boxColor);
        canvas.DrawText(decorator, "abcdifghijklmABC", loc); */
    canvas.PopPencil();
  }
}
