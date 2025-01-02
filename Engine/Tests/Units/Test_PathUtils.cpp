#include <gtest/gtest.h>

#include <filesystem>

#include <Sorex/FileSystem/SxPathUtils.h>

using namespace Sorex;

namespace
{
  struct Test_PathIntstance
  {
    Path str;
    Path root;
    Path dir;
    Path file;
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
                        SRX_PATH("/etc"),
                        SRX_PATH("/etc/fstab"),
                        SRX_PATH("a") },
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

Sorex::TVector<Test_PathIntstance> MakePreferred()
{
  Sorex::TVector<Test_PathIntstance> paths;
  for (Test_PathIntstance path : test_paths)
  {
    path.str.make_preferred();
    path.dir.make_preferred();
    path.file.make_preferred();
    path.root.make_preferred();

    paths.push_back(std::move(path));
  }

  return paths;
}

TEST(PathUtils, SplitPath)
{
  auto paths = MakePreferred();
  for (const Test_PathIntstance& path : paths)
  {
    auto fpath_view = Utils::SplitPath(path.str.native());
    EXPECT_EQ(fpath_view.first, path.dir.native())
      << "Path:'" << path.str.generic_string() << "' Dir: '"
      << path.dir.generic_string() << "' vs '"
      << Path(fpath_view.first).generic_string() << '\'';

    EXPECT_EQ(fpath_view.second, path.file.native())
      << "Path:'" << path.str.generic_string() << "' File: '"
      << path.file.generic_string() << "' vs '"
      << Path(fpath_view.second).generic_string() << '\'';

    const Path parent_path = path.str.parent_path();
    if (!parent_path.native().empty()
        && parent_path.native().back() != Path::preferred_separator)
    {
      EXPECT_EQ(fpath_view.first, parent_path.native())
        << "Parent path for '" << path.str << "' diff: '"
        << Path(fpath_view.first).generic_string() << "' vs std '"
        << parent_path.generic_string() << "'";
    }
  }

  // closing slash
  PathString dir;
  for (const Test_PathIntstance& path : paths)
  {
    dir = path.dir.native();
    if (!path.str.empty())
      dir.push_back(Path::preferred_separator);

    auto fpath_view = Utils::SplitPath(path.str.native(), true);
    EXPECT_EQ(fpath_view.first, dir)
      << "Path:'" << path.str.generic_string() << "' Dir: '"
      << Path(dir).generic_string() << "' vs '"
      << Path(fpath_view.first).generic_string() << '\'';

    EXPECT_EQ(fpath_view.second, path.file.native())
      << "Path:'" << path.str.generic_string() << "' File: '"
      << path.file.generic_string() << "' vs '"
      << Path(fpath_view.second).generic_string() << '\'';

    Path filename = path.str.filename();
    EXPECT_EQ(fpath_view.second, filename);
  }
}

TEST(PathUtils, BaseName)
{
  PathString dir;
  PathString tmp;
  auto       paths = MakePreferred();

  for (const Test_PathIntstance& path : paths)
  {
    PathStringView base = Utils::GetBaseName(path.str.native());
    EXPECT_EQ(base, path.dir.native())
      << "Path:'" << path.str.generic_string() << "' Base: '"
      << path.dir.generic_string() << "' vs '" << Path(base).generic_string()
      << '\'';

    const Path parent_path = path.str.parent_path();
    if (!parent_path.native().empty()
        && parent_path.native().back() != Path::preferred_separator)
    {
      EXPECT_EQ(base, parent_path.native())
        << "Parent path for '" << path.str << "' diff: '"
        << Path(base).generic_string() << "' vs std '"
        << parent_path.generic_string() << "'";
    }


    dir = path.dir.native();
    if (path.str.empty() == false)
      dir.push_back(Path::preferred_separator);

    base = Utils::GetBaseName(path.str.native(), true);
    EXPECT_EQ(base, dir) << "Path:'" << path.str.generic_string() << "' Base: '"
                         << Path(dir).generic_string() << "' vs '"
                         << Path(base).generic_string() << '\'';
  }
}

TEST(PathUtils, RootName)
{
  PathString dir;
  auto       paths = MakePreferred();

  for (const Test_PathIntstance& path : paths)
  {
    PathStringView root = Utils::GetRootName(path.str.native());
    EXPECT_EQ(root, path.root.native())
      << "Path:'" << path.str.generic_string() << "' Root: '"
      << path.root.generic_string() << "' vs '" << Path(root).generic_string()
      << '\'';

    dir = path.root.native();
    if (path.str.empty() == false)
      dir.push_back(Path::preferred_separator);

    root = Utils::GetRootName(path.str.native(), true);
    EXPECT_EQ(root, dir) << "Path:'" << path.str.generic_string() << "' Root: '"
                         << Path(dir).generic_string() << "' vs '"
                         << Path(root).generic_string() << '\'';
  }
}

TEST(PathUtils, Combine)
{
  struct TestData
  {
    TVector<Path> dirs;
    Path          result;
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

  for (TestData data : test_set)
  {
    data.result.make_preferred();
    for (Path& d : data.dirs)
      d.make_preferred();

    Sorex::TVector<PathString> dirs;
    dirs.resize(data.dirs.size());

    std::transform(data.dirs.cbegin(),
                   data.dirs.cend(),
                   dirs.begin(),
                   [](const Path& p) { return p.native(); });

    EXPECT_EQ(Utils::CombinePath(dirs), data.result);
  }
}

// Test cases
TEST(PathUtils, GetFileExtension)
{
  struct TestData
  {
    Path path;
    Path ext;
    bool bSeparator;
  };

  Sorex::TVector<TestData> test_data = {
    { SRX_PATH(""), SRX_PATH(""), false },
    { SRX_PATH("file"), SRX_PATH(""), false },
    { SRX_PATH("file"), SRX_PATH(""), false },
    { SRX_PATH("file."), SRX_PATH(""), false },
    { SRX_PATH("file."), SRX_PATH(""), false },

    { SRX_PATH("file.txt"), SRX_PATH("txt"), false },
    { SRX_PATH("file.txt"), SRX_PATH(".txt"), true },
    { SRX_PATH("/file.txt"), SRX_PATH("txt"), false },
    { SRX_PATH("file.txt/"), SRX_PATH(""), false },

    { SRX_PATH("file.dir/file.png"), SRX_PATH("png"), false },
    { SRX_PATH("file.dir/file.png"), SRX_PATH(".png"), true },

    { SRX_PATH("file.name.with.dots.txt"), SRX_PATH("txt"), false },
    { SRX_PATH("file.name.with.dots.txt"), SRX_PATH(".txt"), true },

    { SRX_PATH("/path/to/file.txt"), SRX_PATH("txt"), false },
    { SRX_PATH("/path/to/file.txt"), SRX_PATH(".txt"), true },

    { SRX_PATH("/path/to/."), SRX_PATH(""), false },
    { SRX_PATH("/path/to/.hidden_file"), SRX_PATH(""), false },

    { SRX_PATH("/path/to/../.."), SRX_PATH(""), false },
    { SRX_PATH("/path/to/../../"), SRX_PATH(""), false },

    { SRX_PATH(".hiddenfile"), SRX_PATH(""), false },
    { SRX_PATH(".hiddenfile.txt"), SRX_PATH("txt"), false }
  };

  for (TestData& data : test_data)
  {
    data.ext.make_preferred();

    PathStringView ext =
      Utils::GetFileExtension(data.path.make_preferred().native(),
                              data.bSeparator);

    EXPECT_EQ(ext, data.ext.native())
      << "Path: '" << data.path.generic_string() << "' ext: '"
      << Path(ext).generic_string() << "' vs '" << data.ext.generic_string()
      << "'";

    const auto file_ext = data.path.extension().native();
    if (file_ext.length() == 1 && file_ext.back() == SRX_PATH('.'))
      continue;

    ext =
      Sorex::Utils::GetFileExtension(data.path.make_preferred().native(), true);

    EXPECT_EQ(ext, file_ext)
      << "Path: '" << data.path.generic_string() << "' ext: '"
      << Path(ext).generic_string() << "' vs '"
      << data.path.extension().generic_string() << "'";
  }
}
