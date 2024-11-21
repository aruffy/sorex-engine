#include <iostream>

#include <Sorex/RuntimeClass.h>
#include <Sorex/Status.h>
#include <Sorex/JournalManager.h>

#include <Sorex/Time.h>

using namespace Sorex;

int main(const int argc, const char* argv[])
{
  Thread::SetMainThread();

  Status status =
    SRX_STATUS_MSG(Sorex::EStatusCode::Busy, "Status Busy {}", argc);
  std::cout << status.DebugMessage() << std::endl;

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
