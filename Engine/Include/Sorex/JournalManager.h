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

#include <spdlog/spdlog.h>

#include "Types.h"
#include "Platform.h"
#include "Status.h"

#ifndef SRX_LOGGING_THREAD_NUM
#  define SRX_LOGGING_THREAD_NUM (2)
#endif

namespace Sorex
{
  // NOTE: array index
  enum class ELogLevel : uint8
  {
    Fatal = 0u,
    Error,
    Warning,
    Info,
    Debug,
    Trace
  };

  class JournalManager final
  {
public:
    static constexpr size_t kMaxLoggerNumber = 4;

    JournalManager(const JournalManager&)            = delete;
    JournalManager& operator=(const JournalManager&) = delete;

    static JournalManager& GetInstance() SRX_NOEXCEPT;

    bool SetLevel(ELogLevel level, int logger = -1) SRX_NOEXCEPT;

private:
    // NOTE: array indecies
    enum ELogger
    {
      Logger_Engine = 0,  ///< Main Engine Logger (main engine thread)
      Logger_Task,        ///< Async Task Logger (other engine threads)
      Logger_User         ///< User Logger
    };

    JournalManager() SRX_NOEXCEPT;

    SRX_INLINE spdlog::logger* GetLogger(const int logger) const SRX_NOEXCEPT;
    static spdlog::level::level_enum ConvLogLevel(ELogLevel level) SRX_NOEXCEPT;

private:
    TArray<TUniquePointer<spdlog::logger>, kMaxLoggerNumber> mLoggers;
  };
}  // namespace
