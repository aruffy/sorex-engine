#include <gtest/gtest.h>

#include <Sorex/Graphics/TextureBitmap.h>

using namespace Sorex::Graphics;

TEST(TextureBitmap, PixelDepth)
{
  EXPECT_EQ(Utils::GetPixelDepth(EPixelFormat::None), 0);
  EXPECT_EQ(Utils::GetPixelDepth(EPixelFormat::A8), 1);
  EXPECT_EQ(Utils::GetPixelDepth(EPixelFormat::ABGR8), 4);
  EXPECT_EQ(Utils::GetPixelDepth(EPixelFormat::ARGB1555), 2);
  EXPECT_EQ(Utils::GetPixelDepth(EPixelFormat::ARGB4), 2);
  EXPECT_EQ(Utils::GetPixelDepth(EPixelFormat::ARGB8), 4);
  EXPECT_EQ(Utils::GetPixelDepth(EPixelFormat::BGR565), 2);
  EXPECT_EQ(Utils::GetPixelDepth(EPixelFormat::BGR8), 3);
  EXPECT_EQ(Utils::GetPixelDepth(EPixelFormat::RGB565), 2);
  EXPECT_EQ(Utils::GetPixelDepth(EPixelFormat::RGB8), 3);
  EXPECT_EQ(Utils::GetPixelDepth(EPixelFormat::RGBA4), 2);
  EXPECT_EQ(Utils::GetPixelDepth(EPixelFormat::RGBA5551), 2);
}

TEST(TextureBitmap, Common)
{
  TextureBitmap bitmap_empty;
  EXPECT_TRUE(bitmap_empty.IsEmpty());
  EXPECT_EQ(bitmap_empty.GetPixelFormat(), EPixelFormat::None);
  EXPECT_EQ(bitmap_empty.GetBytesNumber(), 0);
  EXPECT_EQ(bitmap_empty.GetSize(), Sorex::SizeInt(0, 0));
  EXPECT_FALSE(bitmap_empty.GetSize().IsValid());
  EXPECT_EQ(bitmap_empty.GetPixelDepth(), 0);
  EXPECT_EQ(bitmap_empty.GetScanLine(0).size(), 0);

  const TextureBitmap* cbitmap = &bitmap_empty;
  EXPECT_EQ(cbitmap->GetScanLine(0).size(), 0);

  TextureBitmap bitmap32x32_rgb888(32, 32, EPixelFormat::RGB8);
  cbitmap = &bitmap32x32_rgb888;
  EXPECT_EQ(bitmap32x32_rgb888.GetPixelDepth(), 3);
  EXPECT_FALSE(bitmap32x32_rgb888.IsEmpty());
  EXPECT_EQ(bitmap32x32_rgb888.GetPixelFormat(), EPixelFormat::RGB8);
  EXPECT_EQ(bitmap32x32_rgb888.GetSize(), Sorex::SizeInt(32, 32));
  EXPECT_EQ(bitmap32x32_rgb888.GetBytesNumber(), 32 * 32 * 3);
  EXPECT_EQ(bitmap32x32_rgb888.GetScanLine(0).size(), 32 * 3);
  EXPECT_EQ(cbitmap->GetScanLine(0).size(), 32 * 3);
  EXPECT_EQ(bitmap32x32_rgb888.GetScanLine(31).size(), 32 * 3);
  EXPECT_EQ(cbitmap->GetScanLine(31).size(), 32 * 3);
  EXPECT_EQ(bitmap32x32_rgb888.GetScanLine(32).size(), 0);
  EXPECT_EQ(cbitmap->GetScanLine(32).size(), 0);

  TextureBitmap bitmap_argb;
  cbitmap = &bitmap_argb;
  EXPECT_TRUE(bitmap_argb.IsEmpty());
  EXPECT_EQ(bitmap_argb.GetPixelFormat(), EPixelFormat::None);
  EXPECT_EQ(bitmap_argb.GetBytesNumber(), 0);
  EXPECT_EQ(bitmap_argb.GetSize(), Sorex::SizeInt());
  EXPECT_FALSE(bitmap_argb.GetSize().IsValid());
  EXPECT_EQ(bitmap_argb.GetPixelDepth(), 0);
  EXPECT_EQ(bitmap_argb.GetScanLine(0).size(), 0);
  EXPECT_EQ(cbitmap->GetScanLine(0).size(), 0);

  const Sorex::SizeInt sz32x64(32, 64);
  EXPECT_EQ(bitmap_argb.Resize(sz32x64), 0);
  EXPECT_EQ(bitmap_argb.Resize(sz32x64, EPixelFormat::ARGB4),
            sz32x64.width * sz32x64.height * 2);
  EXPECT_FALSE(bitmap_argb.IsEmpty());
  EXPECT_EQ(bitmap_argb.GetPixelFormat(), EPixelFormat::ARGB4);
  EXPECT_EQ(bitmap_argb.GetBytesNumber(), sz32x64.width * sz32x64.height * 2);
  EXPECT_EQ(bitmap_argb.GetBytesPerLine(), 32 * 2);
  EXPECT_EQ(bitmap_argb.GetSize(), sz32x64);
  EXPECT_EQ(bitmap_argb.GetPixelDepth(), 2);
  EXPECT_EQ(bitmap_argb.GetScanLine(0).size(), bitmap_argb.GetBytesPerLine());
  EXPECT_EQ(bitmap_argb.GetScanLine(64).size(), 0);

  const Sorex::SizeInt sz128x32(128, 32);
  EXPECT_EQ(bitmap_argb.Resize(sz128x32), sz128x32.width * sz128x32.height * 2);
  EXPECT_FALSE(bitmap_argb.IsEmpty());
  EXPECT_EQ(bitmap_argb.GetPixelFormat(), EPixelFormat::ARGB4);
  EXPECT_EQ(bitmap_argb.GetBytesNumber(), sz128x32.width * sz128x32.height * 2);
  EXPECT_EQ(bitmap_argb.GetBytesPerLine(), sz128x32.width * 2);
  EXPECT_EQ(bitmap_argb.GetSize(), sz128x32);
  EXPECT_EQ(bitmap_argb.GetPixelDepth(), 2);
  EXPECT_EQ(bitmap_argb.GetScanLine(0).size(), 128 * 2);
  EXPECT_EQ(bitmap_argb.GetScanLine(20).size(), 128 * 2);
  EXPECT_EQ(bitmap_argb.GetScanLine(32).size(), 0);

  const Sorex::SizeInt sz256x256(256, 256);
  EXPECT_EQ(bitmap_argb.Resize(sz256x256, EPixelFormat::ARGB8),
            sz256x256.width * sz256x256.height * 4);
  EXPECT_FALSE(bitmap_argb.IsEmpty());
  EXPECT_EQ(bitmap_argb.GetPixelFormat(), EPixelFormat::ARGB8);
  EXPECT_EQ(bitmap_argb.GetBytesNumber(),
            sz256x256.width * sz256x256.height * 4);
  EXPECT_EQ(bitmap_argb.GetBytesPerLine(), 256 * 4);
  EXPECT_EQ(bitmap_argb.GetSize(), sz256x256);
  EXPECT_EQ(bitmap_argb.GetPixelDepth(), 4);
  EXPECT_EQ(bitmap_argb.GetScanLine(255).size(), 256 * 4);
  EXPECT_EQ(bitmap_argb.GetScanLine(256).size(), 0);
}
