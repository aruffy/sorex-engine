#pragma once

#include "Types.h"
#include "Status.h"
#include "Assert.h"

namespace Sorex::Utils
{
  /**
   * @brief Computes the hash of the given string view.
   *
   * This function uses a static hash function to generate a hash value
   * for the provided string view.
   *
   * @param str The string view to be hashed.
   * @return The computed hash value of the input string view.
   */
  SRX_API SRX_INLINE hash_t GetHash(const StringView str) SRX_NOEXCEPT
  {
    static const THash<StringView> kStringViewHasger;
    return kStringViewHasger(str);
  }

  /**
   * @brief Checks if the given string starts with the specified prefix.
   *
   * @param str The string to be checked.
   * @param prefix The prefix to check against the start of the string.
   * @return True if the string starts with the prefix, false otherwise.
   */
  SRX_API bool StartWith(StringView str, StringView prefix) SRX_NOEXCEPT;

  namespace Details
  {
    template<typename Char>
    struct CharTraits
    {};

    template<>
    struct CharTraits<Sorex::String::value_type>
    {
      using ValueType = Sorex::String::value_type;

      static constexpr ValueType kNullTerm  = '\0';
      static constexpr ValueType kCharZero  = '0';
      static constexpr ValueType kCharPlus  = '+';
      static constexpr ValueType kCharMinus = '-';
    };

    template<>
    struct CharTraits<Sorex::WString::value_type>
    {
      using ValueType = Sorex::WString::value_type;

      static constexpr ValueType kNullTerm  = L'\0';
      static constexpr ValueType kCharZero  = L'0';
      static constexpr ValueType kCharPlus  = L'+';
      static constexpr ValueType kCharMinus = L'-';
    };

    template<typename T>
    using TEnableIf_Integer =
      std::enable_if_t<std::is_integral_v<T> && !std::is_same_v<T, bool>, T>;

    template<typename T>
    using TEnableIf_SignedInteger =
      std::enable_if<std::is_integral_v<T> && std::is_signed_v<T>
                       && !std::is_same_v<T, bool>,
                     T>;

    template<typename T>
    using TEnableIf_UnsignedInteger =
      std::enable_if<std::is_integral_v<T> && std::is_unsigned_v<T>
                       && !std::is_same_v<T, bool>,
                     T>;

    template<typename T>
    using TEnableIf_FloatingPoint = std::enable_if<std::is_floating_point_v<T>>;

    template<typename T>
    using TEnableIf_Numeric =
      std::enable_if_t<((std::is_integral_v<T> && !std::is_same_v<T, bool>)
                        || std::is_floating_point_v<T>)>;
  }  // namespace Details

  template<typename Int,
           typename Char,
           typename = SRX_TYPENAME Details::TEnableIf_Integer<Int>>
  SRX_API EStatusCode ToInteger(BasicStringView<Char> string,
                                Int&                  result) SRX_NOEXCEPT
  {
    static_assert(sizeof(Char) <= sizeof(int32),
                  "[ToInteger] Invalid character type");

    result = Int(0);
    if (string.empty())
      return EStatusCode::Ok;

    bool           bNegative  = false;
    constexpr bool bUnsingned = std::is_unsigned_v<Int>;

    size_t pos = 0;
    switch (string[0])
    {
    case Details::CharTraits<Char>::kCharMinus:
      if constexpr (bUnsingned)
        return EStatusCode::Invalid_Format;

      bNegative = true;
      SRX_FALLTHROUGH;
    case Details::CharTraits<Char>::kCharPlus:
      pos = 1;
      break;
    }

    const Int kValueLimit    = bNegative ? std::numeric_limits<Int>::min()
                                         : std::numeric_limits<Int>::max();
    const Int kTennerLimit   = kValueLimit / Int(10);
    const Int kRemanderLimit = kValueLimit % Int(10);

    for (const Char ch :
         BasicStringView<Char>(string.data() + pos, string.length() - pos))
    {
      if (!std::isdigit(static_cast<int>(ch)))
        return EStatusCode::Invalid_Argument;

      const int32 n =
        static_cast<int32>(ch) - Details::CharTraits<Char>::kCharZero;

      SRX_CHECK(n <= 10);

      if constexpr (!bUnsingned)
      {
        if (bNegative)
        {
          if (result < kTennerLimit
              || (result == kTennerLimit
                  && static_cast<Int>(-n) < kRemanderLimit))
            return EStatusCode::No_Space;

          result = result * 10 - n;
          continue;
        }
      }

      if (result > kTennerLimit
          || (result == kTennerLimit && static_cast<Int>(n) > kRemanderLimit))
        return EStatusCode::No_Space;

      result = result * 10 + n;
    }

    return EStatusCode::Ok;
  }

  template<typename Int,
           typename Char,
           typename = SRX_TYPENAME Details::TEnableIf_Integer<Int>>
  SRX_API SRX_INLINE EStatusCode ToInteger(const BasicString<Char>& string,
                                           Int& result) SRX_NOEXCEPT
  {
    return ToInteger<Int, Char>(
      BasicStringView<Char>(string.data(), string.length()),
      result);
  }

  template<typename T,
           typename = SRX_TYPENAME Details::TEnableIf_FloatingPoint<T>>
  SRX_API EStatusCode ToFloat(StringView str, T& value) SRX_NOEXCEPT
  {
    if (str.empty())
    {
      value = T{ 0 };
      return EStatusCode::Ok;
    }

    char* endptr;
    errno = 0;

#if (defined(_MSC_VER) && _MSC_VER < 1800)
    value = strtod(str.data(), &endptr);
#else
    if constexpr (std::is_same_v<T, float>)
      value = strtof(str.data(), &endptr);
    else
      value = static_cast<T>(strtod(str.data(), &endptr));
#endif

    if (((*endptr == '\0') || isspace(*endptr)) && errno == 0)
      return EStatusCode::Ok;

    return (errno == ERANGE && value == HUGE_VALF)
             ? EStatusCode::No_Space
             : EStatusCode::Invalid_Format;
  }

  template<typename T,
           typename = SRX_TYPENAME Details::TEnableIf_FloatingPoint<T>>
  SRX_API EStatusCode ToFloat(WStringView wstr, T& value) SRX_NOEXCEPT
  {
    if (wstr.empty())
    {
      value = T{ 0 };
      return EStatusCode::Ok;
    }

    wchar_t* endptr;
    errno = 0;

#if (defined(_MSC_VER) && _MSC_VER < 1800)
    value = wcstod(str.data(), &endptr);
#else
    if constexpr (std::is_same_v<T, float>)
      value = wcstof(wstr.data(), &endptr);
    else
      value = static_cast<T>(wcstod(wstr.data(), &endptr));
#endif

    if (((*endptr == '\0') || isspace(*endptr)) && errno == 0)
      return EStatusCode::Ok;

    return (errno == ERANGE && value == HUGE_VALF)
             ? EStatusCode::No_Space
             : EStatusCode::Invalid_Format;
  }

  template<typename T,
           typename TChar,
           typename = SRX_TYPENAME Details::TEnableIf_FloatingPoint<T>>
  SRX_INLINE EStatusCode ToFloat(const BasicString<TChar>& str,
                                 T&                        value) SRX_NOEXCEPT
  {
    return ToFloat(BasicStringView<TChar>(str.data(), str.length()), value);
  }

  template<typename Char>
  SRX_INLINE float ToFloat(const BasicString<Char>& str) SRX_NOEXCEPT
  {
    float value;
    SRX_VERIFY(ToFloat(BasicStringView<Char>(str.data(), str.length()), value)
               == EStatusCode::Ok);
    return value;
  }


  template<typename T, typename = SRX_TYPENAME Details::TEnableIf_Numeric<T>>
  SRX_INLINE String ToString(const T value) SRX_NOEXCEPT
  {
    return std::to_string(value);
  }

  template<typename T, typename = SRX_TYPENAME Details::TEnableIf_Numeric<T>>
  SRX_INLINE WString ToWString(const T value) SRX_NOEXCEPT
  {
    return std::to_wstring(value);
  }

}  // namespace
