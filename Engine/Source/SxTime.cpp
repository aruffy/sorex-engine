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

#include <Sorex/SxTime.h>
#include <Sorex/Math/SxMaths.h>

#include <chrono>
#include <iomanip>

namespace
{
  using namespace Sorex;

  class ITimeSystem
  {
public:
    virtual ~ITimeSystem() = default;

    /**
     * @brief Retrieves the current system date and time in Coordinated
     * Universal Time (UTC) format.
     *
     * @param time - out parameter of system time (UTC).
     *
     * @return `true` if time is retrieved successfully, else `false`.
     */
    virtual bool GetSystemTime(SystemTime& time) { return false; }

    /**
     * @brief Retrieves the current local date and time.
     *
     * @param time - out parameter of local time.
     *
     * @return Error::No_Error if get time successfully, else code error.
     */
    virtual bool GetLocalTime(SystemTime& time) { return false; }

    /**
     * @brief Retrieves monotonic clock time in milliseconds.
     */
    virtual uint64 GetMonotonicCounter() = 0;

    /**
     * @return Retrieves Unix timestamp in seconds from system clock.
     */
    virtual uint32 GetUnixTime() = 0;

    /**
     * @brief Retrieves the current value of the counter, which is a high
     * resolution time stamp that can be used for time-interval measurements.
     *
     * @return value of steady counter
     */
    virtual uint64 GetSteadyCounter() = 0;

    /**
     * @brief Retrieves the frequency of the performance counter.
     */
    virtual uint64 GetSteadyCounterFrequency() = 0;
  };

  class StandartTimeSystem final: public ITimeSystem
  {
public:
    explicit StandartTimeSystem() SRX_NOEXCEPT {}

    virtual bool   GetSystemTime(SystemTime& time) override;
    virtual bool   GetLocalTime(SystemTime& time) override;
    virtual uint64 GetMonotonicCounter() override;
    virtual uint32 GetUnixTime() override;
    virtual uint64 GetSteadyCounter() override;
    virtual uint64 GetSteadyCounterFrequency() override;
  };

  bool StandartTimeSystem::GetSystemTime(SystemTime& time)
  {
    const std::time_t systime =
      std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    std::tm tm;
#if defined(SOREX_COMPILER_MSVC)
    gmtime_s(&tm, &systime);
#else
    gmtime_r(&systime, &tm);
#endif

    time = SystemTime(tm);
    return true;
  }

  bool StandartTimeSystem::GetLocalTime(SystemTime& time)
  {
    const std::time_t systime =
      std::chrono::system_clock::to_time_t(std::chrono::system_clock::now());

    std::tm tm;
#if defined(SOREX_COMPILER_MSVC)
    localtime_s(&tm, &systime);
#else
    localtime_r(&systime, &tm);
#endif

    time = SystemTime(tm);
    return true;
  }

  uint64 StandartTimeSystem::GetMonotonicCounter()
  {
    const std::chrono::steady_clock::time_point now =
      std::chrono::steady_clock::now();
    return static_cast<uint64>(
      duration_cast<std::chrono::milliseconds>(now.time_since_epoch()).count());
  }

  uint32 StandartTimeSystem::GetUnixTime()
  {
    const auto duration = std::chrono::system_clock::now().time_since_epoch();
    return static_cast<uint32>(
      duration_cast<std::chrono::seconds>(duration).count());
  }

  uint64 StandartTimeSystem::GetSteadyCounter()
  {
    const std::chrono::steady_clock::duration duration =
      std::chrono::steady_clock::now().time_since_epoch();
    return static_cast<uint64>(duration.count());
  }

  uint64 StandartTimeSystem::GetSteadyCounterFrequency()
  {
    return static_cast<uint64>(std::chrono::steady_clock::period::den);
  }

  SRX_INLINE ITimeSystem& GetTimeSystem() SRX_NOEXCEPT
  {
    static StandartTimeSystem _timeSystem;
    return _timeSystem;
  }
}

namespace Sorex
{
  bool operator==(const SystemTime& lhs, const SystemTime& rhs) SRX_NOEXCEPT
  {
    return (lhs.year == rhs.year) && (lhs.month == rhs.month)
           && (lhs.day == rhs.day) && (lhs.hour == rhs.hour)
           && (lhs.minute == rhs.minute) && (lhs.second == rhs.second)
           && (lhs.millisec == rhs.millisec);
  }

  bool operator<(const SystemTime& lhs, const SystemTime& rhs) SRX_NOEXCEPT
  {
    if (lhs.year != rhs.year)
      return lhs.year < rhs.year;

    if (lhs.month != rhs.month)
      return lhs.month < rhs.month;

    if (lhs.day != rhs.day)
      return lhs.day < rhs.day;

    if (lhs.hour != rhs.hour)
      return lhs.hour < rhs.hour;

    if (lhs.minute != rhs.minute)
      return lhs.minute < rhs.minute;

    if (lhs.second != rhs.second)
      return lhs.second < rhs.second;

    return lhs.millisec < rhs.millisec;
  }

  bool Time::GetSystemTime(SystemTime& time) SRX_NOEXCEPT
  {
    return GetTimeSystem().GetSystemTime(time);
  }

  bool Time::GetLocalTime(SystemTime& time) SRX_NOEXCEPT
  {
    return GetTimeSystem().GetLocalTime(time);
  }

  uint64 Time::GetMonotonicCounter() SRX_NOEXCEPT
  {
    return GetTimeSystem().GetMonotonicCounter();
  }

  uint32 Time::GetUnixTime() SRX_NOEXCEPT
  {
    return GetTimeSystem().GetUnixTime();
  }

  uint64 Time::GetSteadyCounter() SRX_NOEXCEPT
  {
    return GetTimeSystem().GetSteadyCounter();
  }

  uint64 Time::GetSteadyCounterFrequency() SRX_NOEXCEPT
  {
    return GetTimeSystem().GetSteadyCounterFrequency();
  }

  std::ostream& operator<<(std::ostream&     ostrm,
                           const SystemTime& stm) SRX_NOEXCEPT
  {
    std::tm tm;
    tm.tm_year  = Math::Clamp<int>(stm.year, 1900, 30827) - 1900;
    tm.tm_mon   = stm.month - 1;
    tm.tm_mday  = stm.day;
    tm.tm_wday  = stm.dayOfWeek;
    tm.tm_hour  = stm.hour;
    tm.tm_min   = stm.minute;
    tm.tm_sec   = stm.second;
    tm.tm_isdst = 0;  // Not daylight saving

    const std::time_t t = std::mktime(&tm);
#ifdef SOREX_COMPILER_MSVC
    struct std::tm localTime;
    if (localtime_s(&localTime, &t) != 0)
    {
      SRX_NOENTRY("timer conv failed");
      return ostrm;
    }
    const std::tm* local = &localTime;
#else
    const std::tm* local = std::localtime(&t);
#endif

    ostrm << std::put_time(local, "%b %d %Y %T");
    return ostrm;
  }
}
