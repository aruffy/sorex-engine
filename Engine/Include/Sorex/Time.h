/**************************************************************************/
/*                         This file is part of:                          */
/*                                SOREX                                   */
/*                 Simple OpenGL Rendering Engine eXtended                */
/**************************************************************************/
/* Copyright (c) 2022 Aleksandr Ershov (Ruffy).                           */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#pragma once

#include "Types.h"
#include "Platform.h"

namespace Sorex
{
  struct SystemTime
  {
    SystemTime() = default;
    explicit SystemTime(const std::tm& time) SRX_NOEXCEPT
      : year(time.tm_year)
      , month(time.tm_mon)
      , day(time.tm_mday)
      , dayOfWeek(time.tm_wday)
      , hour(time.tm_hour)
      , minute(time.tm_min)
      , second(time.tm_sec)
      , millisec(0)
    {}

    int16 year      = 0;
    int16 month     = 0;
    int16 day       = 0;
    int16 dayOfWeek = 0;
    int16 hour      = 0;
    int16 minute    = 0;
    int16 second    = 0;
    int16 millisec  = 0;

    // TODO: Add operator ostream <<
  };
}

namespace Sorex
{
  class Time final
  {
public:
    /**
     * @brief Retrieves the current system date and time in Coordinated
     * Universal Time (UTC) format.
     *
     * @param time - out parameter of system time (UTC).
     * @return Error::None if get time successfully, else code error.
     */
    static bool GetSystemTime(SystemTime& time) SRX_NOEXCEPT;

    /**
     * @brief Retrieves the current local date and time.
     *
     * @param time - out parameter of local time.
     * @return Error::None if get time successfully, else code error.
     */
    static bool GetLocalTime(SystemTime& time) SRX_NOEXCEPT;

    /**
     * @brief Retrieves monotonic clock time in milliseconds.
     */
    static uint64 GetMonotonicCounter() SRX_NOEXCEPT;

    /**
     * @return Retrieves Unix timestamp in seconds from system clock.
     */
    static uint32 GetUnixTime() SRX_NOEXCEPT;

    /**
     * @brief Retrieves the current value of the counter, which is a high
     * resolution time stamp that can be used for time-interval measurements.
     *
     * @return value of steady counter
     */
    static uint64 GetSteadyCounter() SRX_NOEXCEPT;

    /**
     * @brief Retrieves the frequency of the performance counter.
     */
    static uint64 GetSteadyCounterFrequency() SRX_NOEXCEPT;
  };

  bool operator==(const SystemTime& lhs, const SystemTime& rhs) SRX_NOEXCEPT;
  SRX_INLINE bool operator!=(const SystemTime& lhs,
                             const SystemTime& rhs) SRX_NOEXCEPT
  {
    return !(lhs == rhs);
  }

  bool operator<(const SystemTime& lhs, const SystemTime& rhs) SRX_NOEXCEPT;
  SRX_INLINE bool operator>(const SystemTime& lhs,
                            const SystemTime& rhs) SRX_NOEXCEPT
  {
    return rhs < lhs;
  }

  SRX_INLINE bool operator>=(const SystemTime& lhs,
                             const SystemTime& rhs) SRX_NOEXCEPT
  {
    return !(lhs < rhs);
  }

  SRX_INLINE bool operator<=(const SystemTime& lhs,
                             const SystemTime& rhs) SRX_NOEXCEPT
  {
    return !(lhs > rhs);
  }
}
