#include <Sorex/Utils/String.h>

namespace Sorex::Utils
{
  SRX_API bool StartWith(StringView str, StringView prefix) SRX_NOEXCEPT
  {
    if (prefix.size() > str.size())
      return false;

    if (prefix.empty())
      return true;

    return (str.rfind(prefix, 0) == 0);
  }
}  // namespace
