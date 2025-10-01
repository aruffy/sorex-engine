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

#include "CMakeSorexExports.h"

namespace Sorex
{
  /**
   * @brief Enum class representing the debug mode of the application.
   *
   * The debug mode determines which checking functionalities are enabled,
   * including logging, assertions, verification, and validation.
   */
  enum class EDebugMode
  {
    None = 0,    ///< No checks are performed at all.
    Low  = 1,    ///< Non-performance intensive checks: assertion macros,
                 ///< and basic logging (info, warn, error).
    Medium = 2,  ///< Performance-intensive checks: check/verify macros,
                 ///< debug logging, and other checks.
    High = 3     ///< Intensive validation with no concerns for performance.
  };

  enum class ETargetPlatform
  {
    Win32,
    Linux,
    MacOS,
    AppleIOS,
    Android
  };

  enum class ESorexCompiler
  {
    MSVC,
    GNUC,
    GCCE,
    CLANG
  };
}  // namespace

#define SRX_IDLE \
  do             \
  {              \
  } while (0)

#if !defined(SOREX_DEBUG_MODE)
#  define SOREX_DEBUG_LOW (1)
#else
#  if SOREX_DEBUG_MODE == 0
#    define SOREX_DEBUG_NONE (1)
#  else
#    if SOREX_DEBUG_MODE >= 1  // ::Sorex::EDebugMode::Low
#      define SOREX_DEBUG_LOW (1)
#    endif
#    if SOREX_DEBUG_MODE >= 2  // ::Sorex::EDebugMode::Medium
#      define SOREX_DEBUG_MEDIUM (1)
#    endif
#    if SOREX_DEBUG_MODE >= 3  //::Sorex::EDebugMode::High
#      define SOREX_DEBUG_HIGH (1)
#    endif
#  endif
#endif

#if !defined(SOREX_DEBUG_NONE) && defined(SOREX_ENVIRONMENT_DEVELOPMENT)
#  define SOREX_MONITORING (1)
#endif

// Finds current platform
#if defined(__WIN32__) || defined(_WIN32) || defined(TARGET_PLATFORM_WINDOWS)
#  define SOREX_TARGET_PLATFORM (::Sorex::ETargetPlatform::Win32)
#  define SOREX_PLATFORM_WIN32 (1)

#elif defined(__ANDROID__) || defined(TARGET_PLATFORM_ANDROID)
#  define SOREX_TARGET_PLATFORM (::Sorex::ETargetPlatform::Android)
#  define SOREX_PLATFORM_ANDROID (1)

#elif defined(TARGET_PLATFORM_LINUX)
#  define SOREX_TARGET_PLATFORM (::Sorex::ETargetPlatform::Linux)
#  define SOREX_PLATFORM_LINUX (1)

#  include <Sorex/SxLinuxPlatform.h>

#elif defined(__APPLE_CC__) || defined(TARGET_PLATFORM_MACOSX) \
  || defined(TARGET_PLATFORM_IOS)
#  if __ENVIRONMENT_IPHONE_OS_VERSION_MIN_REQUIRED__ >= 60000 \
    || __IPHONE_OS_VERSION_MIN_REQUIRED >= 60000
#    define SOREX_TARGET_PLATFORM (::Sorex::ETargetPlatform::AppleIOS)
#    define SOREX_PLATFORM_IOS (1)
#  else
#    define SOREX_TARGET_PLATFORM (::Sorex::ETargetPlatform::MacOS)
#    define SOREX_PLATFORM_MACOSX (1)
#  endif

#else
#  pragma error "Sorex doesn't support target platform!"
#endif

// Finds the compiler type and version
#if defined(__GCCE__)
#  define SOREX_COMPILER (::Sorex::ESorexCompiler::GCCE)
#  define SOREX_COMPILER_GCCE (1)
#  define SOREX_COMP_VER _MSC_VER

#elif defined(_MSC_VER)
#  define SOREX_COMPILER (::Sorex::ESorexCompiler::MSVC)
#  define SOREX_COMPILER_MSVC (1)
#  define SOREX_COMP_VER _MSC_VER

#elif defined(__clang__)
#  define SOREX_COMPILER (::Sorex::ESorexCompiler::CLANG)
#  define SOREX_COMPILER_CLANG (1)
#  define SOREX_COMP_VER \
    (((__clang_major__) * 100) + (__clang_minor__ * 10) + __clang_patchlevel__)

#elif defined(__GNUC__)
#  define SOREX_COMPILER (::Sorex::ESorexCompiler::GNUC)
#  define SOREX_COMPILER_GNUC (1)
#  define SOREX_COMP_VER \
    (((__GNUC__) * 100) + (__GNUC_MINOR__ * 10) + __GNUC_PATCHLEVEL__)

#else
#  pragma error "Sorex unknown compiler. Abort!"
#endif

#if defined(SOREX_COMPILER_MSVC) && SOREX_COMP_VER >= 1200
#  define SRX_INLINE __forceinline

#elif !defined(__ANDROID__) \
  && (defined(SOREX_COMPILER_GNUC) || defined(SOREX_COMPILER_CLANG))
#  define SRX_INLINE inline __attribute__((always_inline))

#else
#  define SRX_INLINE inline
#endif

#define SRX_NOEXCEPT noexcept
#define SRX_CONSTEVAL consteval
#define SRX_NODISCARD [[nodiscard]]
#define SRX_FALLTHROUGH [[fallthrough]]

// Macros
#ifdef SOREX_COMPILER_MSVC
#  define SRX_TYPENAME typename
#else
#  define SRX_TYPENAME
#endif

#define SRX_LIKELY [[likely]]
#define SRX_UNLIKELY [[unlikely]]

#define SRX_ATOMIC_LOAD(atomic) \
  std::atomic_load_explicit((&(atomic)), std::memory_order_consume)
#define SRX_ATOMIC_STORE(atomic, value) \
  std::atomic_store_explicit((&(atomic)), (value), std::memory_order_release)
