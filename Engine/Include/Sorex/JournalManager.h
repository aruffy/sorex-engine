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
#include "Thread.h"

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

    SRX_INLINE void SetCallback(GetLoggerParamsCallback&& callback) SRX_NOEXCEPT
    {
      mGetLoggerParams = std::move(callback);
    }

    template<
      typename... Args,
      typename Enable = SRX_TYPENAME std::enable_if_t<(sizeof...(Args) != 0)>>
    void            PushRecord(uint8                       loggerId,
                               ELogLevel                   level,
                               fmt::format_string<Args...> format,
                               Args&&... args) const;
    SRX_INLINE void PushRecord(uint8      loggerId,
                               ELogLevel  level,
                               StringView message) const;

private:
    JournalManager() SRX_NOEXCEPT;

    SRX_INLINE spdlog::logger* GetLogger(const uint8 logger) const SRX_NOEXCEPT;
    static spdlog::level::level_enum ConvertLevel(ELogLevel level) SRX_NOEXCEPT;

    TUniquePointer<spdlog::logger> CreateLogger(StringView          name,
                                                const LoggerParams& params,
                                                Status& status) SRX_NOEXCEPT;

private:
    ELogLevel                                                mLevel;
    TArray<TUniquePointer<spdlog::logger>, kMaxLoggerNumber> mLoggers;
    THashMap<String, spdlog::sink_ptr>                       mSinks;

    GetLoggerParamsCallback mGetLoggerParams;
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

  SRX_INLINE spdlog::logger* JournalManager::GetLogger(
    const uint8 loggerId) const SRX_NOEXCEPT
  {
    if (loggerId < kMaxLoggerNumber)
      return mLoggers[loggerId].get();

    return nullptr;
  }

  template<typename... Args, typename Enable>
  void JournalManager::PushRecord(uint8                       loggerId,
                                  ELogLevel                   level,
                                  fmt::format_string<Args...> format,
                                  Args&&... args) const
  {
    if (spdlog::logger* const logger = GetLogger(loggerId))
      logger->log(ConvertLevel(level),
                  std::move(format),
                  std::forward<Args>(args)...);
  }

  SRX_INLINE void JournalManager::PushRecord(uint8      loggerId,
                                             ELogLevel  level,
                                             StringView message) const
  {
    if (auto logger = GetLogger(loggerId))
      logger->log(ConvertLevel(level), message);
  }
}  // namespace

#define SOREX_JOURNAL_RECORD(loggerId, level, format, ...)  \
  Sorex::JournalManager::GetInstance().PushRecord(loggerId, \
                                                  level,    \
                                                  format,   \
                                                  ##__VA_ARGS__)

#define SRX_USER_LOG(level, format, ...)                   \
  SOREX_JOURNAL_RECORD(Sorex::JournalManager::kUserLogger, \
                       level,                              \
                       format,                             \
                       ##__VA_ARGS__)

#define LOG_ERROR(format, ...) \
  SRX_USER_LOG(Sorex::ELogLevel::Error, format, ##__VA_ARGS__)
#define LOG_WARN(format, ...) \
  SRX_USER_LOG(Sorex::ELogLevel::Warning, format, ##__VA_ARGS__)
#define LOG_INFO(format, ...) \
  SRX_USER_LOG(Sorex::ELogLevel::Info, format, ##__VA_ARGS__)
#define LOG_DEBUG(format, ...) \
  SRX_USER_LOG(Sorex::ELogLevel::Debug, format, ##__VA_ARGS__)
#define LOG_TRACE(format, ...) \
  SRX_USER_LOG(Sorex::ELogLevel::Trace, format, ##__VA_ARGS__)

#define SRX_ENGINE_LOG(level, format, ...)                       \
  SOREX_JOURNAL_RECORD((Sorex::Thread::IsMainThread()            \
                          ? Sorex::JournalManager::kEngineLogger \
                          : Sorex::JournalManager::kTaskLogger), \
                       level,                                    \
                       format,                                   \
                       ##__VA_ARGS__)

#ifndef SOREX_DEBUG_NONE
#  define SRX_ERROR(format, ...) \
    SRX_ENGINE_LOG(Sorex::ELogLevel::Error, format, ##__VA_ARGS__)
#  define SRX_WARN(format, ...) \
    SRX_ENGINE_LOG(Sorex::ELogLevel::Warning, format, ##__VA_ARGS__)
#  define SRX_INFO(format, ...) \
    SRX_ENGINE_LOG(Sorex::ELogLevel::Info, format, ##__VA_ARGS__)

#  ifdef SOREX_DEBUG_MEDIUM
#    define SRX_DEBUG(format, ...) \
      SRX_ENGINE_LOG(Sorex::ELogLevel::Debug, format, ##__VA_ARGS__)
#  else
#    define SRX_DEBUG(format, ...) SRX_IDLE
#  endif

#  ifdef SOREX_DEBUG_HIGH
#    define SRX_TRACE(format, ...) \
      SRX_ENGINE_LOG(Sorex::ELogLevel::Trace, format, ##__VA_ARGS__)
#  else  // RUFFY_ENGINE_DEBUG
#    define SRX_TRACE(format, ...) SRX_IDLE
#  endif

#else  // SOREX_DEBUG_NONE

#  define SRX_ERROR(format, ...) SRX_IDLE
#  define SRX_WARN(format, ...) SRX_IDLE
#  define SRX_INFO(format, ...) SRX_IDLE
#  define SRX_DEBUG(format, ...) SRX_IDLE
#  define SRX_TRACE(format, ...) SRX_IDLE
#endif
