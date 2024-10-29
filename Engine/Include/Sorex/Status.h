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
  namespace Concept
  {
    template<typename T>
    concept ErrorCategory =
      std::is_same_v<std::remove_cv_t<T>, std::error_category>
      && std::is_error_code_enum_v<typename T::EStatusCode>;
  }

  template<Concept::ErrorCategory T>
  class SRX_API TStatus
  {
    using EStatusCode = T::errcode_t;

public:
    SRX_INLINE static const T& GetErrorCategory() SRX_NOEXCEPT;

    SRX_INLINE bool Success() const SRX_NOEXCEPT;
    SRX_INLINE bool HasError() const SRX_NOEXCEPT { return !Success(); }

    SRX_INLINE EStatusCode GetCode() const SRX_NOEXCEPT
    {
      return static_cast<EStatusCode>(mCode.value());
    }

    SRX_INLINE String GetMessage() const SRX_NOEXCEPT
    {
      return mCode.message();
    }

    void   Reset() SRX_NOEXCEPT;
    String ToString() const SRX_NOEXCEPT;

private:
    std::error_code mCode;

#ifdef SOREX_DEBUG_LOW
    String mMessage;  // TODO: make as optional to save for OK status
#endif

#ifdef SOREX_DEBUG_MEDIUM
    String mFilename;
    int    mLine = 0;

    SRX_INLINE void SetDebugInfo(const char* filename, int line) SRX_NOEXCEPT;
#endif
  };

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

  // TODO: Implementation correction
  template<Concept::ErrorCategory T>
  String TStatus<T>::ToString() const SRX_NOEXCEPT
  {
    std::stringstream ss;
    ss << (Success() ? "[OK]" : "[ERROR]") << " Status:\n";

#ifdef SOREX_DEBUG_MEDIUM
    if (!mFilename.empty())
      ss << "\tContext: " << mFilename << ":" << mLine << "\n";
#endif

    ss << "\tCode: " << '<' << std::hex << "0x" << mCode.value() << "> "
       << std::dec << '\n'
       << "\tText: " << mCode.message() << '\n';

#ifdef SOREX_DEBUG_LOW
    if (!mMessage.empty())
      ss << "\tMessage: " << mMessage;
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

  enum class EStatusCode : errcode_t
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

  class StatusCodeCategory final: public std::error_category
  {
public:
    using EStatusCode = Sorex::EStatusCode;

    const char* name() const noexcept override;
    std::string message(int errcode) const override;
  };

}  // namespace
