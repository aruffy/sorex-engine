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

#include <Sorex/JournalManager.h>

#include <Sorex/Assert.h>

#include <spdlog/async.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/dist_sink.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>

namespace
{
  using namespace Sorex;
  constexpr size_t kAsyncQueueSize = 8192U;
  const String     kTermSinkId     = "Sorex:TerminalSinkId";


#ifdef SOREX_DEBUG_NONE
  bool GetDefaultLoggerParams(uint8_t,
                              JournalManager::LoggerParams&) SRX_NOEXCEPT
  {
    return false;
  }
#else
  bool GetDefaultLoggerParams(uint8_t                       id,
                              JournalManager::LoggerParams& params) SRX_NOEXCEPT
  {
#  if defined(SOREX_DEBUG_HIGH)
    const ELogLevel logLevel = ELogLevel::Trace;
#  elif defined(SOREX_DEBUG_MEDIUM)
    const ELogLevel logLevel = ELogLevel::Debug;
#  else
    const ELogLevel logLevel = ELogLevel::Info;
#  endif

    params.level        = logLevel;
    params.bTermLogging = true;
    // NOTE: It doesn't make a lot of sense as the terminal sink is alway
    // multithreading
    params.bMultithreading = (id != JournalManager::kEngineLogger);
    // TODO: Enable file logging to the executable dir
    params.filename.clear();
    params.rotationParams.reset();

    return true;
  }
#endif
}  // namespace

namespace Sorex
{
  JournalManager::JournalManager() SRX_NOEXCEPT
    : mLevel(ELogLevel::Info)
    , mGetLoggerParams(GetDefaultLoggerParams)
  {
#ifdef SOREX_DEBUG_HIGH
    mLevel = ELogLevel::Trace;
#elif defined(SOREX_DEBUG_MEDIUM)
    mLevel = ELogLevel::Debug;
#else
    mLevel = ELogLevel::Info;
#endif

    spdlog::init_thread_pool(kAsyncQueueSize, (size_t)SOREX_LOG_THREAD_NUM);
    spdlog::set_level(ConvertLevel(mLevel));

#ifndef SOREX_DEBUG_NONE
    RegisterLogger<kEngineLogger>("Engine");
    RegisterLogger<kTaskLogger>("Engine:Thread");
    RegisterLogger<kUserLogger>("Application");
#endif
  }

  JournalManager::~JournalManager()
  {
    spdlog::drop_all();
    spdlog::shutdown();
  }

  JournalManager& JournalManager::GetInstance() SRX_NOEXCEPT
  {
    static JournalManager instance;
    return instance;
  }

  spdlog::level::level_enum JournalManager::ConvertLevel(ELogLevel level)
    SRX_NOEXCEPT
  {
    static constexpr TArray<spdlog::level::level_enum, 7> kSpdLevels{
      spdlog::level::critical, spdlog::level::err,   spdlog::level::warn,
      spdlog::level::info,     spdlog::level::debug, spdlog::level::trace
    };

    const unsigned index = static_cast<unsigned>(level);
    return kSpdLevels.size() > index ? kSpdLevels[index] : spdlog::level::trace;
  }

  TUniquePointer<spdlog::logger> JournalManager::CreateLogger(
    StringView          name,
    const LoggerParams& params,
    Status&             status) SRX_NOEXCEPT
  {
    TVector<spdlog::sink_ptr> sinks;
    const auto                logLevel = ConvertLevel(params.level);

    if (name.empty())
    {
      status =
        SRX_STATUS_MSG(EStatusCode::Invalid_Argument, "invalid logger name");
      return nullptr;
    }

    if (params.bTermLogging)
    {
      if (auto it = mSinks.find(kTermSinkId); it == mSinks.end())
      {
#ifdef SOREX_DEBUG_MEDIUM
        spdlog::sink_ptr termSink =
          std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
#else
        spdlog::sink_ptr termSink =
          std::make_shared<spdlog::sinks::stdout_sink_mt>();
#endif
        termSink->set_level(logLevel);
        termSink->set_pattern("[%t] [%T.%e] [%^%n @%l%$] %v");
        mSinks[kTermSinkId] = termSink;

        sinks.push_back(std::move(termSink));
      }
      else
      {
        sinks.push_back(it->second);
      }
    }

    if (!params.filename.empty())
    {
      if (auto it = mSinks.find(params.filename); it != mSinks.end())
      {
        sinks.push_back(it->second);
      }
      else
      {
        spdlog::sink_ptr fileSink;
        if (params.rotationParams.has_value())
        {
          const auto& rotation = params.rotationParams.value();
          if (params.bMultithreading)
            fileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
              params.filename,
              rotation.fileSize,
              rotation.fileNumber);
          else
            fileSink = std::make_shared<spdlog::sinks::rotating_file_sink_st>(
              params.filename,
              rotation.fileSize,
              rotation.fileNumber);
        }
        else  // basic file
        {
          if (params.bMultithreading)
            fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
              params.filename,
              true);
          else
            fileSink = std::make_shared<spdlog::sinks::basic_file_sink_st>(
              params.filename,
              true);
        }

        if (fileSink)
        {
          fileSink->set_level(logLevel);
          fileSink->set_pattern("[%d/%b/%Y %T.%e] [%n @%l] %v");

          mSinks[params.filename] = fileSink;
          sinks.push_back(std::move(fileSink));
        }
      }
    }

    if (sinks.empty())
    {
      status =
        SRX_STATUS_MSG(EStatusCode::Invalid_Argument, "invalid logger params");
      return nullptr;
    }

    auto logger = MakeUnique<spdlog::logger>(std::string(name),
                                             std::begin(sinks),
                                             std::end(sinks));

#ifdef SOREX_DEBUG_MEDIUM
    auto flushLevel = spdlog::level::info;
#else
    auto flushLevel = spdlog::level::warn;
#endif

    logger->set_level(logLevel);
    logger->flush_on(flushLevel);

    if (!status.Ok())
      status = SRX_OK;

    return logger;
  }

}  // namespace
