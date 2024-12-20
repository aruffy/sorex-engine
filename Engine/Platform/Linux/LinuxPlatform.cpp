#include <Sorex/SxAssert.h>

namespace Sorex::Platform
{
  SRX_API int OnAssertionFailed(const char* message,
                                const char* file,
                                int         line) SRX_NOEXCEPT
  {
    if (message && file)
    {
      fprintf(stderr,
              "[FATAL] %s:%i Assertion failed: '%s'\n",
              file,
              line,
              message);
      fflush(stderr);
    }

    return 0;
  }

  SRX_API void OnCheckFailed(const char* message,
                             const char* file,
                             int         line) SRX_NOEXCEPT
  {
    if (message && file)
    {
      fprintf(stderr,
              "[FATAL] %s:%i Check failed: '%s'\n",
              file,
              line,
              message);
      fflush(stderr);
    }
  }
}
