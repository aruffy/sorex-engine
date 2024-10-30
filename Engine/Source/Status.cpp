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

#include <Sorex/Status.h>

namespace Sorex::Details
{
  const char* StatusCodeCategory::name() const noexcept
  {
    return "Status::Code";
  }

  std::string StatusCodeCategory::message(int errcode) const
  {
    switch (static_cast<EStatusCode>(errcode))
    {
    case EStatusCode::Ok:
      return "Ok";
    case EStatusCode::Invalid_Argument:
      return "Invalid Argument";
    case EStatusCode::Invalid_State:
      return "Invalid State";
    case EStatusCode::Invalid_Format:
      return "Invalid Format";
    case EStatusCode::Not_Available:
      return "Not Available";
    case EStatusCode::Not_Permitted:
      return "Not Permitted";
    case EStatusCode::Not_Supported:
      return "Not Supported";
    case EStatusCode::Not_Implemented:
      return "Not Implemented";
    case EStatusCode::Not_Unique:
      return "Not Unique";
    case EStatusCode::Not_Found:
      return "Not Found";
    case EStatusCode::No_Memory:
      return "No Memory";
    case EStatusCode::No_Space:
      return "No Space";
    case EStatusCode::No_Data:
      return "No Data";
    case EStatusCode::Access_Error:
      return "Access Error";
    case EStatusCode::Out_Of_Range:
      return "Out Of Range";
    case EStatusCode::Bad_File:
      return "Bad File";
    case EStatusCode::Bad_Address:
      return "Bad Address";
    case EStatusCode::Busy:
      return "Busy";
    case EStatusCode::Try_Again:
      return "Try Again";
    default:
      return "Unknown error";
    }
  }

}  // namespace
//
