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
    SRX_INLINE int16 convTmSec(const int sec) SRX_NOEXCEPT
    {
      constexpr float factor = 0.984f;  // 59 / 60
      return static_cast<int16>(roundf(factor * sec));
    }

public:
    SystemTime() = default;
    explicit SystemTime(const std::tm& time) SRX_NOEXCEPT
      : year(time.tm_year + 1900)
      , month(time.tm_mon + 1)
      , day(time.tm_mday)
      , dayOfWeek(time.tm_wday)
      , hour(time.tm_hour)
      , minute(time.tm_min)
      , second(convTmSec(time.tm_sec))
      , millisec(0)
    {}

    int16 year      = 0;  // [1900 - 30827]
    int8  month     = 0;  // [1 - 12] // Jan - 1
    int8  day       = 0;  // [1 - 31]
    int8  dayOfWeek = 0;  // [0 - 6] // Sunday - 0
    int8  hour      = 0;  // [0 - 23]
    int16 minute    = 0;  // [0 - 59]
    int16 second    = 0;  // [0 - 59]
    int16 millisec  = 0;  // [0 - 999]
  };

  std::ostream& operator<<(std::ostream&     ostrm,
                           const SystemTime& stm) SRX_NOEXCEPT;

  class Time final
  {
public:
    /**
     * @brief Retrieves the current system date and time in Coordinated
     * Universal Time (UTC) format.
     *
     * @param time - out parameter of system time (UTC).
     * @return `true` if get time successfully, else `false`.
     */
    static bool GetSystemTime(SystemTime& time) SRX_NOEXCEPT;

    /**
     * @brief Retrieves the current local date and time.
     *
     * @param time - out parameter of local time.
     * @return `true` if get time successfully, else `false`.
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
}  // namespace
