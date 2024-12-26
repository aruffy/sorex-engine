#include <gtest/gtest.h>

#include <Sorex/FileSystem/SxDirectory.h>
#include <Sorex/FileSystem/SxPathUtils.h>

#include <FileSystem/SxStaticDirectory.h>

using namespace Sorex;

TEST(StaticDirectory, Main)
{
  FileSystem::Path path(__FILE__);
  const auto       dirPath = path.parent_path().make_preferred();

  FileSystem::StaticDirectory dir(dirPath);

  EXPECT_EQ(dir.IndexFiles(), SRX_OK);

  TVector<FileSystem::FileIndex> files;
  dir.GetFiles((dirPath / ""), files);

  ASSERT_NE(files.size(), 0) << "Dir '" << dirPath << "' hasn't files";
  Status status;
  auto   file = dir.OpenFile(files[0], EAccessMode::Read, &status);
  EXPECT_NE(file, nullptr);
  EXPECT_EQ(status, SRX_OK);

  file.reset();

  const auto [fileStatus, fileIndex] = dir.GetFileIndex(path);

  EXPECT_EQ(fileStatus, FileSystem::EFileStatus::Existent);
  ASSERT_TRUE(fileIndex.has_value());

  file.reset();
  EXPECT_EQ(file, nullptr);

  file = dir.OpenFile(fileIndex.value(), EAccessMode::Read, &status);
  EXPECT_NE(file, nullptr);
  EXPECT_EQ(status, SRX_OK);
}
