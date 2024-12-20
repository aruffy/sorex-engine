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

#include <concepts>
#include <system_error>

#include <spdlog/fmt/fmt.h>

#include "SxTypes.h"
#include "SxPlatform.h"

namespace Sorex
{
  enum class EStatusCode
  {
    Ok       = 0,
    No_Error = Ok,
    Invalid_Argument,
    Invalid_State,
    Invalid_Format,
    Not_Available,
    Not_Permitted,
    Not_Supported,
    Not_Implemented,
    Not_Unique,
    Not_Found,
    No_Memory,
    No_Space,
    No_Data,
    Access_Error,
    Out_Of_Range,
    Bad_File,
    Bad_Address,
    Busy,
    Canceled,
    Interrupted,
    Try_Again
  };

  namespace Details
  {
    class StatusCodeCategory final: public std::error_category
    {
  public:
      typedef Sorex::EStatusCode EStatusCode;

      const char* name() const noexcept override;
      std::string message(int errcode) const override;

      SRX_NODISCARD static SRX_INLINE std::error_code create(
        const EStatusCode errcode = static_cast<EStatusCode>(0)) SRX_NOEXCEPT
      {
        static const StatusCodeCategory errorCategory;
        return std::error_code(static_cast<int>(errcode), errorCategory);
      }

  private:
      StatusCodeCategory() = default;
    };
  }

  namespace Concept
  {
    template<typename T>
    concept ErrorCategory =
      std::is_base_of_v<std::error_category, std::remove_cv_t<T>>
      && std::is_error_code_enum_v<typename T::EStatusCode>;
  }
}

namespace std
{
  template<>
  struct is_error_code_enum<Sorex::EStatusCode>: public true_type
  {};
}

namespace Sorex
{
  inline std::error_code make_error_code(Sorex::EStatusCode errcode)
  {
    return Details::StatusCodeCategory::create(errcode);
  }

  class SRX_API Status final
  {
public:
    SRX_INLINE Status() SRX_NOEXCEPT;
    template<typename T,
             typename Enable =
               SRX_TYPENAME std::enable_if_t<std::is_error_code_enum_v<T>>>
    explicit SRX_INLINE Status(const T errcode)
      : Status(make_error_code(errcode))
    {}

    SRX_INLINE explicit Status(std::error_code&& errcode) SRX_NOEXCEPT;

    template<typename T>
    SRX_NODISCARD static SRX_INLINE
      std::enable_if_t<std::is_error_code_enum_v<T>, Status>
      Create(const T errcode) SRX_NOEXCEPT
    {
      return Status(make_error_code(errcode));
    }

#ifdef SOREX_DEBUG_MEDIUM
    template<typename T>
    SRX_NODISCARD static SRX_INLINE
      std::enable_if_t<std::is_error_code_enum_v<T>, Status>
      Create(const T errcode, std::string_view msg) SRX_NOEXCEPT
    {
      return Status(make_error_code(errcode), msg);
    }

    template<typename T, typename... Args>
    SRX_NODISCARD static SRX_INLINE
      std::enable_if_t<std::is_error_code_enum_v<T> && (sizeof...(Args)),
                       Status>
      Create(const T                     errcode,
             fmt::format_string<Args...> fmt,
             Args&&... args) SRX_NOEXCEPT
    {
      return Status(make_error_code(errcode),
                    fmt::format(std::move(fmt), std::forward<Args>(args)...));
    }
#endif
#ifdef SOREX_DEBUG_HIGH
    SRX_INLINE Status SetDebugInfo(const char* filename, int line) SRX_NOEXCEPT;
#endif

    SRX_INLINE bool Ok() const SRX_NOEXCEPT { return !mCode.value(); }

    SRX_INLINE int   GetCode() const SRX_NOEXCEPT { return mCode.value(); }
    SRX_INLINE const std::error_category& GetCategory() SRX_NOEXCEPT;

#ifndef SOREX_DEBUG_MEDIUM
    SRX_INLINE String ToString() const SRX_NOEXCEPT { return mCode.message(); }
#else
    SRX_INLINE String ToString() const SRX_NOEXCEPT { return DebugMessage(); }
#endif

    /**
     * @brief Resets the status.
     *
     * This function clears the current error code and resets any associated
     * message or debugging information.
     */
    void Reset() SRX_NOEXCEPT;

    SRX_INLINE operator bool() const SRX_NOEXCEPT { return Ok(); }

private:
    /**
     * @brief Generates a debug message indicating the status of the operation.
     *
     * This function constructs a string representation of the status, including
     * the success state, error code, and associated message. It may also
     * include additional context information if available.
     *
     * @return A string containing the debug message.
     */
    String DebugMessage() const SRX_NOEXCEPT;

#ifdef SOREX_DEBUG_MEDIUM
    SRX_INLINE Status(std::error_code&& errcode,
                      std::string&&     msg) SRX_NOEXCEPT;
    SRX_INLINE Status(std::error_code&&       errcode,
                      const std::string_view& msg) SRX_NOEXCEPT;
#endif
private:
    std::error_code mCode;

#ifdef SOREX_DEBUG_MEDIUM
    TOptional<String> mMessage;
#endif

#ifdef SOREX_DEBUG_HIGH
    String mFilename;
    int    mLine = 0;
#endif
  };

  //---------------------------------------------------------------------------

  SRX_INLINE Status::Status() SRX_NOEXCEPT
    : Status(make_error_code(EStatusCode::Ok))
  {}

  SRX_INLINE Status::Status(std::error_code&& errcode) SRX_NOEXCEPT
    : mCode(std::move(errcode))
  {}

#ifdef SOREX_DEBUG_MEDIUM
  SRX_INLINE Status::Status(std::error_code&& errcode,
                            std::string&&     msg) SRX_NOEXCEPT
    : mCode(std::move(errcode))
    , mMessage(std::move(msg))
  {}
  SRX_INLINE Status::Status(std::error_code&&       errcode,
                            const std::string_view& msg) SRX_NOEXCEPT
    : mCode(std::move(errcode))
    , mMessage(msg)
  {}
#endif

  SRX_INLINE const std::error_category& Status::GetCategory() SRX_NOEXCEPT
  {
    return mCode.category();
  }

#ifdef SOREX_DEBUG_HIGH
  SRX_INLINE Status Status::SetDebugInfo(const char* filename,
                                         int         line) SRX_NOEXCEPT
  {
    if (filename)
    {
      mFilename.assign(filename);
      mLine = line;
    }

    return *this;
  }
#endif

}  // namespace


#define SRX_STATUS(errcode) (Sorex::Status((errcode)))
#define SRX_OK SRX_STATUS(EStatusCode::Ok)

#if defined(SOREX_DEBUG_HIGH)
#  define SRX_STATUS_MSG(errcode, format, ...)                \
    Sorex::Status::Create((errcode), (format), ##__VA_ARGS__) \
      .SetDebugInfo(__FILE__, __LINE__)
#elif defined(SOREX_DEBUG_MEDIUM)
#  define SRX_STATUS_MSG(errcode, format, ...) \
    Sorex::Status::Create((errcode), (format), ##__VA_ARGS__)
#else
#  define SRX_STATUS_MSG(errcode, format, ...) SRX_STATUS(errcode)
#endif
