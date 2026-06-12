#include <gtest/gtest.h>

#include <array>
#include <cstdio>

#include <Sorex/FileSystem/SxFile.h>

using namespace Sorex;

#define FILE_CHECK_NOERR(file) EXPECT_EQ((file).GetStatus().GetCode(), 0);

TEST(SxFile, Common)
{
  // NULL FILE
  Status               status;
  TUniquePointer<File> null_file =
    File::Open("", EAccessMode::Read, File::EOpenMode::Binary, &status);
  EXPECT_EQ(null_file, nullptr);
  EXPECT_NE(status, SRX_OK);

  File no_file("");
  ASSERT_FALSE(no_file.IsOpen());
  ASSERT_NE(no_file.GetStatus(), SRX_OK);

  // THIS FILE
  File file(__FILE__);
  ASSERT_TRUE(file.IsOpen());
  ASSERT_EQ(file.GetName(), __FILE__);
  FILE_CHECK_NOERR(file);

  const auto length = file.GetLength();
  ASSERT_GT(length, 0);
  FILE_CHECK_NOERR(file);
  EXPECT_EQ(file.GetPosition(), 0);
  FILE_CHECK_NOERR(file);

  // Seek
  ASSERT_GT(length, 40);
  ssize_t pos = 0;
  for (; pos < 10; pos += 2)
  {
    ASSERT_TRUE(file.Seek(pos, ESeekMode::Begin));
    FILE_CHECK_NOERR(file);
    ASSERT_EQ(file.GetPosition(), pos);
  }

  for (; pos < 40; ++pos)
  {
    ASSERT_TRUE(file.Seek(1, ESeekMode::Current));
    FILE_CHECK_NOERR(file);
    ASSERT_EQ(file.GetPosition(), pos - 1);
  }

  EXPECT_TRUE(file.Seek(0, ESeekMode::End));
  FILE_CHECK_NOERR(file);
  EXPECT_EQ(file.GetPosition(), length);

  ASSERT_TRUE(file.Reset());
  FILE_CHECK_NOERR(file);
  ASSERT_EQ(file.GetPosition(), 0);
  FILE_CHECK_NOERR(file);

  const auto    bufsz = std::min<ssize_t>(length, 64);
  TVector<byte> buffer(bufsz, 0);
  EXPECT_EQ(buffer.size(), bufsz);

  EXPECT_EQ(file.Read(buffer), buffer.size());
  FILE_CHECK_NOERR(file);
  EXPECT_EQ(file.GetPosition(), buffer.size());
  FILE_CHECK_NOERR(file);

  EXPECT_EQ(file.Read({}), 0);
  buffer.resize(length + 0xff);
  EXPECT_EQ(file.Read(buffer), length - bufsz);

  EXPECT_TRUE(Utils::CheckBitmask(EAccessMode::Write, EAccessMode::Write));
  EXPECT_FALSE(Utils::CheckBitmask(EAccessMode::Read, EAccessMode::Write));
  EXPECT_TRUE(Utils::CheckBitmask(EAccessMode::ReadWrite, EAccessMode::Read));
  EXPECT_TRUE(Utils::CheckBitmask(EAccessMode::ReadWrite, EAccessMode::Write));

  ASSERT_TRUE(file.Reset());
  FILE_CHECK_NOERR(file);
  ASSERT_EQ(file.GetPosition(), 0);
  FILE_CHECK_NOERR(file);

  EXPECT_EQ(file.Write(buffer), SRX_WRITE_ERROR);
  EXPECT_EQ(file.ReadAll(buffer), length);
}
