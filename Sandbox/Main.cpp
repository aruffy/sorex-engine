#include <iostream>

#include <Sorex/RuntimeClass.h>
#include <Sorex/Status.h>

using namespace Sorex;

int main(const int argc, const char* argv[])
{
  Status status =
    SRX_STATUS_MSG(Sorex::EStatusCode::Busy, "Status Busy {}", argc);
  std::cout << status.DebugMessage() << std::endl;

  std::cout << "[Sorex] Sandbox::Main Start." << std::endl;
  return 0;
}
