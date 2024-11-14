#include <iostream>

#include <Sorex/RuntimeClass.h>
#include <Sorex/Status.h>
#include <Sorex/JournalManager.h>

using namespace Sorex;

int main(const int argc, const char* argv[])
{
  Status status =
    SRX_STATUS_MSG(Sorex::EStatusCode::Busy, "Status Busy {}", argc);
  std::cout << status.DebugMessage() << std::endl;

  // FIXME: Assert
  /* SRX_ASSERT(JournalManager::GetInstance()
               .RegisterLogger<JournalManager::kEngineLogger>("Engine", true)
               .Ok()); */

  if (!(JournalManager::GetInstance()
          .RegisterLogger<JournalManager::kEngineLogger>("Engine", true)
          .Ok()))
    return 1;

  std::cout << "[Sorex] Sandbox::Main Start." << std::endl;
  return 0;
}
