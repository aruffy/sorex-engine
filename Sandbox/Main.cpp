#include <iostream>

#include <Sorex/SxRuntimeClass.h>
#include <Sorex/SxStatus.h>
#include <Sorex/SxJournalManager.h>

#include <Sorex/SxTime.h>
#include <Sorex/SxLauncher.h>
#include <Sorex/SxDirector.h>
#include <Sorex/Graphics/SxCanvas.h>

#include <Sorex/SxDesktopLauncher.h>

using namespace Sorex;

class MyDirector final: public Director
{
  public:
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
  }
};

int main(const int argc, const char* argv[])
{
  if (!(JournalManager::GetInstance()
          .RegisterLogger<JournalManager::kEngineLogger>("MyEngine", true)
          .Ok()))
    return 1;

  JournalManager::GetInstance().PushRecord(JournalManager::kEngineLogger,
                                           ELogLevel::Info,
                                           "Hello, world!");
  JournalManager::GetInstance().PushRecord(JournalManager::kEngineLogger,
                                           ELogLevel::Warning,
                                           "Oops code:{} text:{}",
                                           400,
                                           "BAD_REQUEST");

  Platform::DesktopLauncher().Run<MyDirector>();
  return 0;
}
