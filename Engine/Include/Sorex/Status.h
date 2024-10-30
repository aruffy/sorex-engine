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

#include "Types.h"
#include "Platform.h"

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
  template<Concept::ErrorCategory T>
  class SRX_API TStatus
  {
public:
    using EStatusCode = T::EStatusCode;

    SRX_INLINE TStatus() SRX_NOEXCEPT;
    SRX_INLINE explicit TStatus(EStatusCode errcode) SRX_NOEXCEPT;

    SRX_INLINE static const T& GetErrorCategory() SRX_NOEXCEPT;

    SRX_INLINE bool Success() const SRX_NOEXCEPT;
    SRX_INLINE bool HasError() const SRX_NOEXCEPT { return !Success(); }

    SRX_INLINE EStatusCode GetCode() const SRX_NOEXCEPT
    {
      return static_cast<EStatusCode>(mCode.value());
    }

    SRX_INLINE String ToString() const SRX_NOEXCEPT { return mCode.message(); }

    void   Reset() SRX_NOEXCEPT;
    String DebugMessage() const SRX_NOEXCEPT;

private:
    std::error_code mCode;

#ifdef SOREX_DEBUG_LOW
    TOptional<String> mMessage;
#endif

#ifdef SOREX_DEBUG_MEDIUM
    String mFilename;
    int    mLine = 0;

    SRX_INLINE void SetDebugInfo(const char* filename, int line) SRX_NOEXCEPT;
#endif
  };

  using Status = TStatus<Details::StatusCodeCategory>;

  //---------------------------------------------------------------------------

  template<Concept::ErrorCategory T>
  SRX_INLINE TStatus<T>::TStatus() SRX_NOEXCEPT
    : TStatus(static_cast<EStatusCode>(0))
  {}

  template<Concept::ErrorCategory T>
  SRX_INLINE TStatus<T>::TStatus(EStatusCode errcode) SRX_NOEXCEPT
    : mCode(static_cast<int>(errcode), GetErrorCategory())
  {}

  template<Concept::ErrorCategory T>
  SRX_INLINE const T& TStatus<T>::GetErrorCategory() SRX_NOEXCEPT
  {
    static const T errorCategory;
    return errorCategory;
  }

  template<Concept::ErrorCategory T>
  SRX_INLINE bool TStatus<T>::Success() const SRX_NOEXCEPT
  {
    return static_cast<bool>(mCode);
  }

  template<Concept::ErrorCategory T>
  void TStatus<T>::Reset() SRX_NOEXCEPT
  {
    mCode.assign(0, GetErrorCategory());

#ifdef SOREX_DEBUG_LOW
    mMessage.reset();
#endif
#ifdef SOREX_DEBUG_MEDIUM
    mFilename.clear();
    mLine = 0;
#endif
  }

  template<Concept::ErrorCategory T>
  String TStatus<T>::DebugMessage() const SRX_NOEXCEPT
  {
    std::stringstream ss;
    ss << (Success() ? "[OK]" : "[ERROR]") << " Status:\n";

    ss << "\tCode: " << '<' << std::hex << "0x" << mCode.value() << "> "
       << std::dec << '\n'
       << "\tText: " << mCode.message() << '\n';

#ifdef SOREX_DEBUG_LOW
    if (mMessage.has_value() && !mMessage.value().empty())
      ss << "\tMessage: " << mMessage.value() << "\n";
#endif
#ifdef SOREX_DEBUG_MEDIUM
    if (!mFilename.empty())
      ss << "\tContext: " << mFilename << ":" << mLine;
#endif

    return ss.str();
  }

#ifdef SOREX_DEBUG_MEDIUM
  template<Concept::ErrorCategory T>
  SRX_INLINE void TStatus<T>::SetDebugInfo(const char* filename,
                                           int         line) SRX_NOEXCEPT
  {
    if (filename)
    {
      mFilename.assign(filename);
      mLine = line;
    }
  }
#endif

  inline std::error_code make_error_code(Sorex::EStatusCode errcode)
  {
    return std::error_code(
      static_cast<int>(errcode),
      Sorex::TStatus<Sorex::Details::StatusCodeCategory>::GetErrorCategory());
  }
}  // namespace
