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
#include <spdlog/fmt/fmt.h>

#include "Types.h"
#include "Platform.h"
#include "Status.h"

#ifndef SOREX_LOG_THREAD_NUM
#  define SOREX_LOG_THREAD_NUM (2)
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
    struct LoggerParams
    {
      ELogLevel level           = ELogLevel::Info;
      bool      bMultithreading = false;  ///< Enable multithreading for logger
      bool bTermLogging = true;  ///< Flush log messages to the standart output;

      struct FileRotation
      {
        uint32 fileSize   = 5 * 1048576;
        uint8  fileNumber = 3;
      };

      String filename;  ///< If not empty will write log messages to the file;
      TOptional<FileRotation>
        rotationParams;  ///< If specify the file rotating will be applyed;
    };

    /**
     * @brief Callback invoked when JournalManager is about to create a new
     * logger.
     *
     * @param loggerId An integer representing the new logger identifier that is
     * going to be created.
     * @param loggerParams A reference to LoggerParams that will hold the out
     * parameters for the new logger.
     *
     * @return true to allow the creation of the new logger, false to forbid
     * creation.
     */
    typedef std::function<bool(uint8, LoggerParams&)> GetLoggerParamsCallback;

    /**
     * @enum ELogger
     * @brief Enumeration of default loggers used in the engine.
     *
     * This enumeration defines the different types of defaults loggers that can
     * be registered by the engine. The availability of these loggers may
     * depend on compile-time definitions: SRX_DEBUG_NONE prevent loggers to be
     * registred.
     *
     * These loggers uses RegisterLogger function: if the
     * GetLoggerParamsCallback return `false` the loggers will not be created;
     *
     */
    enum ELogger : uint8
    {
      kEngineLogger = 0u,  ///< Main Engine Logger (main engine thread)
      kTaskLogger,         ///< Async Task Logger (other engine threads)
      kUserLogger,         ///< User Logger
      kCustomLogger        ///< Not be registerd by default (user defined)
    };

public:
    static constexpr uint8 kMaxLoggerNumber = 4;

    ~JournalManager();

    JournalManager(const JournalManager&)            = delete;
    JournalManager& operator=(const JournalManager&) = delete;

    static JournalManager& GetInstance() SRX_NOEXCEPT;

    template<uint8 kLoggerId>
    Status RegisterLogger(StringView loggerName, bool bReplace = true);

    template<uint8 kLoggerId>
    SRX_INLINE void SetLogger(TUniquePointer<spdlog::logger>&& logger)
      SRX_NOEXCEPT;

    template<
      typename... Args,
      typename Enable = SRX_TYPENAME std::enable_if_t<(sizeof...(Args) != 0)>>
    void            PushRecord(uint8                       loggerId,
                               ELogLevel                   level,
                               fmt::format_string<Args...> format,
                               Args&&... args);
    SRX_INLINE void PushRecord(uint8      loggerId,
                               ELogLevel  level,
                               StringView message);

private:
    JournalManager() SRX_NOEXCEPT;

    spdlog::logger* GetLogger(const uint8 logger) const SRX_NOEXCEPT;
    static spdlog::level::level_enum ConvLogLevel(ELogLevel level) SRX_NOEXCEPT;

    TUniquePointer<spdlog::logger> CreateLogger(StringView          name,
                                                const LoggerParams& params,
                                                Status& status) SRX_NOEXCEPT;

private:
    GetLoggerParamsCallback                                  mGetLoggerParams;
    TArray<TUniquePointer<spdlog::logger>, kMaxLoggerNumber> mLoggers;
    spdlog::sink_ptr                                         mTermSink;
  };

  template<uint8 kLoggerId>
  Status JournalManager::RegisterLogger(StringView loggerName, bool bReplace)
  {
    static_assert(kLoggerId < kMaxLoggerNumber, "invalid logger index");

    if (!bReplace && mLoggers[kLoggerId] != nullptr)
      return SRX_STATUS_MSG(EStatusCode::Not_Unique,
                            "try to overrite logger {}",
                            kLoggerId);

    LoggerParams params;
    if (mGetLoggerParams && mGetLoggerParams(kLoggerId, params))
    {
      Status status;
      auto   logger = CreateLogger(loggerName, params, status);
      if (logger)
        mLoggers[kLoggerId] = std::move(logger);

      return status;
    }

    return SRX_STATUS(EStatusCode::Not_Permitted);
  }

  template<uint8 kLoggerId>
  SRX_INLINE void JournalManager::SetLogger(
    TUniquePointer<spdlog::logger>&& logger) SRX_NOEXCEPT
  {
    static_assert(kLoggerId < kMaxLoggerNumber, "invalid logger index");
    mLoggers[kLoggerId] = std::move(logger);
  }

  template<typename... Args, typename Enable>
  void JournalManager::PushRecord(uint8                       loggerId,
                                  ELogLevel                   level,
                                  fmt::format_string<Args...> format,
                                  Args&&... args)
  {
    if (auto logger = GetLogger(loggerId))
      logger->log(ConvLogLevel(level),
                  std::move(format),
                  std::forward<Args>(args)...);
  }

  SRX_INLINE void JournalManager::PushRecord(uint8      loggerId,
                                             ELogLevel  level,
                                             StringView message)
  {
    if (auto logger = GetLogger(loggerId))
      logger->log(ConvLogLevel(level), message);
  }
}  // namespace
