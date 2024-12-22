#include <gtest/gtest.h>

#include <FileSystem/SxPathUtils.h>

using namespace Sorex;

namespace
{
  struct Test_PathIntstance
  {
    String str;
    String root;
    String dir;
    String file;
  };

  const TArray<Test_PathIntstance, 27> test_paths = {
    Test_PathIntstance{ "", "", "", "" },
    Test_PathIntstance{ "/", "", "", "" },
    Test_PathIntstance{ "~/dir/fn.txt", "~", "~/dir", "fn.txt" },
    Test_PathIntstance{ "../../file.txt", "..", "../..", "file.txt" },
    Test_PathIntstance{ "/initrd.img", "", "", "initrd.img" },
    Test_PathIntstance{ "/home", "", "", "home" },
    Test_PathIntstance{ "/home/", "/home", "/home", "" },
    Test_PathIntstance{ "/home/user/.config/file.txt",
                        "/home",
                        "/home/user/.config",
                        "file.txt" },
    Test_PathIntstance{ "D:/", "D:", "D:", "" },
    Test_PathIntstance{ "C:/Documents/Newsletters/Summer2018.pdf",
                        "C:",
                        "C:/Documents/Newsletters",
                        "Summer2018.pdf" },
    Test_PathIntstance{
      "G:/Shared drives/Team Drive/project-folder/Documentation/",
      "G:",
      "G:/Shared drives/Team Drive/project-folder/Documentation",
      "" },
    Test_PathIntstance{
      "C:/Users/User/Documents/ProjectFolder/ProjectFile.docx",
      "C:",
      "C:/Users/User/Documents/ProjectFolder",
      "ProjectFile.docx" },
    Test_PathIntstance{ "D:/Downloads/Software/Windows10.iso",
                        "D:",
                        "D:/Downloads/Software",
                        "Windows10.iso" },
    Test_PathIntstance{ "E:/Music/Music.Library/Songs/Song1.mp3",
                        "E:",
                        "E:/Music/Music.Library/Songs",
                        "Song1.mp3" },
    Test_PathIntstance{ "F:/Pictures/Picture Library/Picture1.jpg",
                        "F:",
                        "F:/Pictures/Picture Library",
                        "Picture1.jpg" },
    Test_PathIntstance{ "G:/Videos/MovieLibrary/Movie1.mp4",
                        "G:",
                        "G:/Videos/MovieLibrary",
                        "Movie1.mp4" },
    Test_PathIntstance{ "/usr/local/bin/executable.sh",
                        "/usr",
                        "/usr/local/bin",
                        "executable.sh" },
    Test_PathIntstance{ "/var/log/apache2/access.log",
                        "/var",
                        "/var/log/apache2",
                        "access.log" },
    Test_PathIntstance{ "/etc/passwd/", "/etc", "/etc/passwd", "" },
    Test_PathIntstance{ "/usr/share/doc/bash/examples/startup-files/dot.bashrc",
                        "/usr",
                        "/usr/share/doc/bash/examples/startup-files",
                        "dot.bashrc" },
    Test_PathIntstance{ "/usr/local/lib/libexample.so",
                        "/usr",
                        "/usr/local/lib",
                        "libexample.so" },
    Test_PathIntstance{ "/usr/include/linux/if_ether.h",
                        "/usr",
                        "/usr/include/linux",
                        "if_ether.h" },
    Test_PathIntstance{ "/usr/share/fonts/truetype/dejavu/DejaVuSans.ttf",
                        "/usr",
                        "/usr/share/fonts/truetype/dejavu",
                        "DejaVuSans.ttf" },
    Test_PathIntstance{ "/var/www/html/index.html",
                        "/var",
                        "/var/www/html",
                        "index.html" },
    Test_PathIntstance{ "/etc/fstab/a", "/etc", "/etc/fstab", "a" },
    Test_PathIntstance{ "/home/user/Documents/file.txt",
                        "/home",
                        "/home/user/Documents",
                        "file.txt" },
    Test_PathIntstance{ "/opt/google/chrome/chrome.exe",
                        "/opt",
                        "/opt/google/chrome",
                        "chrome.exe" },
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
              PathString(SRX_PATH("G:/Folder/../Folder")) }
  };

  for (const TestData& data : test_set)
  {
    EXPECT_EQ(Utils::CombinePath(data.dirs), data.result);
  }
}
