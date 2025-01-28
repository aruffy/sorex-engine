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
    canvas.DrawLine(Point(10, 10), Point(10, 200), Color::Purple);
  }
};

int main(const int argc, const char* argv[])
{
  Thread::SetMainThread();

  Status status =
    SRX_STATUS_MSG(Sorex::EStatusCode::Busy, "Status Busy {}", argc);
  std::cout << status.ToString() << std::endl;

  // FIXME: Assert
  /* SRX_ASSERT(JournalManager::GetInstance()
               .RegisterLogger<JournalManager::kEngineLogger>("Engine", true)
               .Ok()); */

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

  SRX_INFO("Hello, from main thread (MACRO)");

  Platform::DesktopLauncher().Run<MyDirector>();

  SystemTime stm;
  Time::GetLocalTime(stm);
  std::cout << "Time: " << stm << std::endl;

  Sorex::Thread thr;
  thr.Execute([]() {
    std::stringstream ss;
    ss << std::this_thread::get_id();
    SRX_DEBUG("Debug message from a thread {} num {}", ss.str(), 124);
  });

  std::cout << "[Sorex] Sandbox::Main Start." << std::endl;
  return 0;
}
