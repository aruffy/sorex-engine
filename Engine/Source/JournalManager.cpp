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

#include <Sorex/JournalManager.h>

#include <spdlog/async.h>
#include <spdlog/sinks/stdout_sinks.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/dist_sink.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/sinks/rotating_file_sink.h>

namespace
{
  constexpr size_t kAsyncQueueSize = 8192U;
}  // namespace

namespace Sorex
{
  JournalManager::JournalManager() SRX_NOEXCEPT
  {
    spdlog::init_thread_pool(kAsyncQueueSize, (size_t)SOREX_LOG_THREAD_NUM);

#ifdef SOREX_DEBUG_HIGH
    spdlog::set_level(spdlog::level::trace);
#elif defined(SOREX_DEBUG_MEDIUM)
    spdlog::set_level(spdlog::level::debug);
#else
    spdlog::set_level(spdlog::level::info);
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

  spdlog::level::level_enum JournalManager::ConvLogLevel(ELogLevel level)
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
    const LoggerParams& params) SRX_NOEXCEPT
  {
    TVector<spdlog::sink_ptr> sinks;

    if (params.bTermLogging)
    {
      spdlog::sink_ptr termSink =
        params.bTermColor
          ? std::make_shared<spdlog::sinks::stdout_color_sink_st>()
          : std::make_shared<spdlog::sinks::stdout_sink_st>();

      termSink->set_pattern("[%T.%e] [%n %^@%l%$] %v");
      sinks.push_back(std::move(termSink));
    }

    return nullptr;
  }

}  // namespace
