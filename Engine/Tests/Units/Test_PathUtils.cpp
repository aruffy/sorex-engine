#include <gtest/gtest.h>

#include <Sorex/FileSystem/SxPathUtils.h>

using namespace Sorex;

namespace
{
  struct Test_PathIntstance
  {
    PathString str;
    PathString root;
    PathString dir;
    PathString file;
  };

  const TArray<Test_PathIntstance, 27> test_paths = {
    Test_PathIntstance{ SRX_PATH(""),
                        SRX_PATH(""),
                        SRX_PATH(""),
                        SRX_PATH("") },
    Test_PathIntstance{ SRX_PATH("/"),
                        SRX_PATH(""),
                        SRX_PATH(""),
                        SRX_PATH("") },
    Test_PathIntstance{ SRX_PATH("~/dir/fn.txt"),
                        SRX_PATH("~"),
                        SRX_PATH("~/dir"),
                        SRX_PATH("fn.txt") },
    Test_PathIntstance{ SRX_PATH("../../file.txt"),
                        SRX_PATH(".."),
                        SRX_PATH("../.."),
                        SRX_PATH("file.txt") },
    Test_PathIntstance{ SRX_PATH("/initrd.img"),
                        SRX_PATH(""),
                        SRX_PATH(""),
                        SRX_PATH("initrd.img") },
    Test_PathIntstance{ SRX_PATH("/home"),
                        SRX_PATH(""),
                        SRX_PATH(""),
                        SRX_PATH("home") },
    Test_PathIntstance{ SRX_PATH("/home/"),
                        SRX_PATH("/home"),
                        SRX_PATH("/home"),
                        SRX_PATH("") },
    Test_PathIntstance{ SRX_PATH("/home/user/.config/file.txt"),
                        SRX_PATH("/home"),
                        SRX_PATH("/home/user/.config"),
                        SRX_PATH("file.txt") },
    Test_PathIntstance{ SRX_PATH("D:/"),
                        SRX_PATH("D:"),
                        SRX_PATH("D:"),
                        SRX_PATH("") },
    Test_PathIntstance{ SRX_PATH("C:/Documents/Newsletters/Summer2018.pdf"),
                        SRX_PATH("C:"),
                        SRX_PATH("C:/Documents/Newsletters"),
                        SRX_PATH("Summer2018.pdf") },
    Test_PathIntstance{
      SRX_PATH("G:/Shared drives/Team Drive/project-folder/Documentation/"),
      SRX_PATH("G:"),
      SRX_PATH("G:/Shared drives/Team Drive/project-folder/Documentation"),
      SRX_PATH("") },
    Test_PathIntstance{
      SRX_PATH("C:/Users/User/Documents/ProjectFolder/ProjectFile.docx"),
      SRX_PATH("C:"),
      SRX_PATH("C:/Users/User/Documents/ProjectFolder"),
      SRX_PATH("ProjectFile.docx") },
    Test_PathIntstance{ SRX_PATH("D:/Downloads/Software/Windows10.iso"),
                        SRX_PATH("D:"),
                        SRX_PATH("D:/Downloads/Software"),
                        SRX_PATH("Windows10.iso") },
    Test_PathIntstance{ SRX_PATH("E:/Music/Music.Library/Songs/Song1.mp3"),
                        SRX_PATH("E:"),
                        SRX_PATH("E:/Music/Music.Library/Songs"),
                        SRX_PATH("Song1.mp3") },
    Test_PathIntstance{ SRX_PATH("F:/Pictures/Picture Library/Picture1.jpg"),
                        SRX_PATH("F:"),
                        SRX_PATH("F:/Pictures/Picture Library"),
                        SRX_PATH("Picture1.jpg") },
    Test_PathIntstance{ SRX_PATH("G:/Videos/MovieLibrary/Movie1.mp4"),
                        SRX_PATH("G:"),
                        SRX_PATH("G:/Videos/MovieLibrary"),
                        SRX_PATH("Movie1.mp4") },
    Test_PathIntstance{ SRX_PATH("/usr/local/bin/executable.sh"),
                        SRX_PATH("/usr"),
                        SRX_PATH("/usr/local/bin"),
                        SRX_PATH("executable.sh") },
    Test_PathIntstance{ SRX_PATH("/var/log/apache2/access.log"),
                        SRX_PATH("/var"),
                        SRX_PATH("/var/log/apache2"),
                        SRX_PATH("access.log") },
    Test_PathIntstance{ SRX_PATH("/etc/passwd/"),
                        SRX_PATH("/etc"),
                        SRX_PATH("/etc/passwd"),
                        SRX_PATH("") },
    Test_PathIntstance{
      SRX_PATH("/usr/share/doc/bash/examples/startup-files/dot.bashrc"),
      SRX_PATH("/usr"),
      SRX_PATH("/usr/share/doc/bash/examples/startup-files"),
      SRX_PATH("dot.bashrc") },
    Test_PathIntstance{ SRX_PATH("/usr/local/lib/libexample.so"),
                        SRX_PATH("/usr"),
                        SRX_PATH("/usr/local/lib"),
                        SRX_PATH("libexample.so") },
    Test_PathIntstance{ SRX_PATH("/usr/include/linux/if_ether.h"),
                        SRX_PATH("/usr"),
                        SRX_PATH("/usr/include/linux"),
                        SRX_PATH("if_ether.h") },
    Test_PathIntstance{
      SRX_PATH("/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf"),
      SRX_PATH("/usr"),
      SRX_PATH("/usr/share/fonts/truetype/dejavu"),
      SRX_PATH("DejaVuSans.ttf") },
    Test_PathIntstance{ SRX_PATH("/var/www/html/index.html"),
                        SRX_PATH("/var"),
                        SRX_PATH("/var/www/html"),
                        SRX_PATH("index.html") },
    Test_PathIntstance{ SRX_PATH("/etc/fstab/a"),
                        ("/etc"),
                        ("/etc/fstab"),
                        ("a") },
    Test_PathIntstance{ SRX_PATH("/home/user/Documents/file.txt"),
                        SRX_PATH("/home"),
                        SRX_PATH("/home/user/Documents"),
                        SRX_PATH("file.txt") },
    Test_PathIntstance{ SRX_PATH("/opt/google/chrome/chrome.exe"),
                        SRX_PATH("/opt"),
                        SRX_PATH("/opt/google/chrome"),
                        SRX_PATH("chrome.exe") },
  };
}

TEST(PathUtils, SplitPath)
{
  TPair<String, String>         fpath;
  TPair<StringView, StringView> fpath_view;

  for (const Test_PathIntstance& path : test_paths)
  {
    fpath_view = Utils::SplitPath(path.str);
    ASSERT_EQ(fpath_view.first, path.dir)
      << "Path:'" << path.str << "' Dir: '" << path.dir << "' vs '"
      << fpath_view.first << '\'';
    ASSERT_EQ(fpath_view.second, path.file)
      << "Path:'" << path.str << "' File: '" << path.file << "' vs '"
      << fpath_view.second << '\'';

    Utils::SplitPath(path.str, fpath);
    ASSERT_EQ(fpath.first, path.dir);
    ASSERT_EQ(fpath.second, path.file);
  }

  // closing slash
  String dir;
  for (const Test_PathIntstance& path : test_paths)
  {
    dir = path.dir;
    if (!path.str.empty())
      dir.push_back('/');

    fpath_view = Utils::SplitPath(path.str, true);
    ASSERT_EQ(fpath_view.first, dir)
      << "Path:'" << path.str << "' Dir: '" << dir << "' vs '"
      << fpath_view.first << '\'';
    ASSERT_EQ(fpath_view.second, path.file)
      << "Path:'" << path.str << "' File: '" << path.file << "' vs '"
      << fpath_view.second << '\'';

    Utils::SplitPath(path.str, fpath, true);
    ASSERT_EQ(fpath.first, dir);
    ASSERT_EQ(fpath.second, path.file);
  }
}

TEST(PathUtils, BaseName)
{
  String dir;
  String tmp;
  for (const Test_PathIntstance& path : test_paths)
  {
    StringView base = Utils::GetBaseName(path.str);
    ASSERT_EQ(base, path.dir) << "Path:'" << path.str << "' Base: '" << path.dir
                              << "' vs '" << base << '\'';

    Utils::GetBaseName(path.str, tmp);
    ASSERT_EQ(tmp, path.dir);

    dir = path.dir;
    if (path.str.empty() == false)
      dir.push_back('/');

    base = Utils::GetBaseName(path.str, true);
    ASSERT_EQ(base, dir) << "Path:'" << path.str << "' Base: '" << dir
                         << "' vs '" << base << '\'';

    Utils::GetBaseName(path.str, tmp, true);
    ASSERT_EQ(tmp, dir);
  }
}

TEST(PathUtils, RootName)
{
  String dir;
  String tmp;
  for (const Test_PathIntstance& path : test_paths)
  {
    StringView root = Utils::GetRootName(path.str);
    ASSERT_EQ(root, path.root) << "Path:'" << path.str << "' Root: '"
                               << path.root << "' vs '" << root << '\'';

    Utils::GetRootName(path.str, tmp);
    ASSERT_EQ(tmp, path.root);

    dir = path.root;
    if (path.str.empty() == false)
      dir.push_back('/');

    root = Utils::GetRootName(path.str, true);
    ASSERT_EQ(root, dir) << "Path:'" << path.str << "' Root: '" << dir
                         << "' vs '" << root << '\'';

    Utils::GetRootName(path.str, tmp, true);
    ASSERT_EQ(tmp, dir);
  }
}

TEST(PathUtils, Combine)
{
  struct TestData
  {
    TVector<PathString> dirs;
    PathString          result;
  };

  const TVector<TestData> test_set = {
    TestData{ { SRX_PATH(""), SRX_PATH(""), SRX_PATH("") }, PathString() },
    TestData{ { SRX_PATH("a"), SRX_PATH("b"), SRX_PATH("c") },
              PathString(SRX_PATH("a/b/c")) },
    TestData{ { SRX_PATH("/"), (""), ("") }, PathString() },
    TestData{
      { SRX_PATH("/"), SRX_PATH("a"), SRX_PATH("//"), SRX_PATH("b/c//") },
      PathString(SRX_PATH("/a/b/c")) },
    TestData{ { SRX_PATH(""), SRX_PATH("/"), SRX_PATH("") }, PathString() },
    TestData{ { SRX_PATH("/"), SRX_PATH("/"), SRX_PATH("/") }, PathString() },
    TestData{ { SRX_PATH("/var"), SRX_PATH("/path/"), SRX_PATH("file.txt") },
              PathString(SRX_PATH("/var/path/file.txt")) },
    TestData{
      { SRX_PATH("/usr/local"), SRX_PATH("bin"), SRX_PATH("executable.sh") },
      PathString(SRX_PATH("/usr/local/bin/executable.sh")) },
    TestData{ { SRX_PATH("/usr/local") }, PathString(SRX_PATH("/usr/local")) },
    TestData{ { SRX_PATH("/usr/") }, PathString(SRX_PATH("/usr")) },
    TestData{ { SRX_PATH("/var/"),
                SRX_PATH("/log/"),
                SRX_PATH("/apache2/access.log/") },
              PathString(SRX_PATH("/var/log/apache2/access.log")) },
    TestData{
      { SRX_PATH("F:/Pictures/Picture Library"), SRX_PATH("Picture1.jpg") },
      PathString(SRX_PATH("F:/Pictures/Picture Library/Picture1.jpg")) },
    TestData{ { SRX_PATH("G:/Videos/MovieLibrary/Movie1.mp4") },
              PathString(SRX_PATH("G:/Videos/MovieLibrary/Movie1.mp4")) },
    TestData{ { SRX_PATH("G:"),
                SRX_PATH("Folder"),
                SRX_PATH(".."),
                SRX_PATH("Folder") },
              PathString(SRX_PATH("G:/Folder/../Folder")) },
    TestData{ { SRX_PATH("G://"),
                SRX_PATH("//Project//"),
                SRX_PATH("/"),
                SRX_PATH("Name//"),
                SRX_PATH("/S///"),
                SRX_PATH("File.txt") },
              SRX_PATH("G:/Project/Name/S/File.txt") }
  };

  for (const TestData& data : test_set)
  {
    EXPECT_EQ(Utils::CombinePath(data.dirs), data.result);
  }
}


// Test cases
TEST(PathUtils, GetFileExtension)
{
  EXPECT_EQ(Utils::GetFileExtension(SRX_PATH("")), SRX_PATH(""));
  EXPECT_EQ(Utils::GetFileExtension(SRX_PATH("file")), SRX_PATH(""));
  EXPECT_EQ(Utils::GetFileExtension(SRX_PATH("file"), true), SRX_PATH(""));
  EXPECT_EQ(Utils::GetFileExtension(SRX_PATH("file.")), SRX_PATH(""));
  EXPECT_EQ(Utils::GetFileExtension(SRX_PATH("file."), true), SRX_PATH(""));

  EXPECT_EQ(Utils::GetFileExtension(SRX_PATH("file.txt")), SRX_PATH("txt"));
  EXPECT_EQ(Utils::GetFileExtension(SRX_PATH("file.txt"), true),
            SRX_PATH(".txt"));
  EXPECT_EQ(Utils::GetFileExtension(SRX_PATH("/file.txt")), SRX_PATH("txt"));
  EXPECT_EQ(Utils::GetFileExtension(SRX_PATH("file.txt/")), SRX_PATH(""));

  EXPECT_EQ(Utils::GetFileExtension(SRX_PATH("file.dir/file.png")),
            SRX_PATH("png"));
  EXPECT_EQ(Utils::GetFileExtension(SRX_PATH("file.dir/file.png"), true),
            SRX_PATH(".png"));

  EXPECT_EQ(Utils::GetFileExtension(SRX_PATH("file.name.with.dots.txt")),
            SRX_PATH("txt"));
  EXPECT_EQ(Utils::GetFileExtension(SRX_PATH("file.name.with.dots.txt"), true),
            (".txt"));

  EXPECT_EQ(Utils::GetFileExtension(SRX_PATH("/path/to/file.txt")),
            SRX_PATH("txt"));
  EXPECT_EQ(Utils::GetFileExtension(SRX_PATH("/path/to/file.txt"), true),
            SRX_PATH(".txt"));

  EXPECT_EQ(Utils::GetFileExtension(SRX_PATH("/path/to/.")), SRX_PATH(""));
  EXPECT_EQ(Utils::GetFileExtension(SRX_PATH("/path/to/.hidden_file")),
            SRX_PATH(""));

  EXPECT_EQ(Utils::GetFileExtension(SRX_PATH("/path/to/../..")), SRX_PATH(""));

  EXPECT_EQ(Utils::GetFileExtension(SRX_PATH(".hiddenfile")), SRX_PATH(""));
  EXPECT_EQ(Utils::GetFileExtension(SRX_PATH(".hiddenfile.txt")),
            SRX_PATH("txt"));
}
