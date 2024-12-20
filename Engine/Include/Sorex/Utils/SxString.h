#pragma once

#include <Sorex/Types.h>
#include <Sorex/Status.h>
#include <Sorex/SxAssert.h>

namespace Sorex::Utils
{
  inline static const String kEmptyString;

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
  SRX_API SRX_INLINE String ToString(const T value) SRX_NOEXCEPT
  {
    return std::to_string(value);
  }

  template<typename T, typename = SRX_TYPENAME Details::TEnableIf_Numeric<T>>
  SRX_API SRX_INLINE WString ToWString(const T value) SRX_NOEXCEPT
  {
    return std::to_wstring(value);
  }

  /**
   * @brief Splits a string into parts based on a specified separator.
   *
   * This function takes a string and divides it into substrings, storing the
   * results in a vector. The substrings are determined by the occurrences of
   * the specified separator character.
   *
   * @tparam Char The character type of the string.
   * @param str The input string to be split.
   * @param separator The character used to separate the substrings.
   * @param parts A vector that will hold the resulting substrings
   * @return The number of substrings added to the parts vector.
   */
  template<typename Char>
  SRX_API size_t Split(const BasicString<Char>&        str,
                       Char                            separator,
                       TVector<BasicStringView<Char>>& parts) SRX_NOEXCEPT
  {
    const size_t count = parts.size();
    size_t       begin = 0;
    size_t       i     = 0;

    const size_t len = str.length();
    for (; i < len; ++i)
    {
      if (str[i] == separator)
      {
        if (i > begin)
          parts.emplace_back(&str[begin], i - begin);

        begin = i + 1;
      }
    }

    if (i > begin)
      parts.emplace_back(&str[begin], len - begin);

    return parts.size() - count;
  }

  /**
   * @brief Trims whitespace characters from the left side of the given string
   * view.
   *
   * @tparam TChar The character type of the string view.
   * @param str The string view to be trimmed.
   * @return A new string view with leading whitespace removed.
   */
  template<typename TChar>
  SRX_API BasicStringView<TChar> TrimLeft(const BasicStringView<TChar> str)
    SRX_NOEXCEPT
  {
    size_t       start = 0;
    const size_t len   = str.length();
    while (start < len && std::isspace(str[start]))
    {
      ++start;
    }

    return (start < len) ? str.substr(start) : BasicStringView<TChar>();
  }

  /**
   * @brief Trims whitespace characters from the left side of the given string
   * view.
   *
   * @tparam TChar The character type of the string view.
   * @param str The string view to be trimmed.
   * @return A new string view with leading whitespace removed.
   */
  template<typename TChar>
  SRX_API SRX_INLINE BasicStringView<TChar> TrimLeft(
    const BasicString<TChar>& str) SRX_NOEXCEPT
  {
    return TrimLeft(BasicStringView<TChar>(str));
  }

  /**
   * @brief Trims whitespace characters from the right end of the given string
   * view.
   *
   * @param str The string view to be trimmed. It is a view of the original
   * string from which whitespace characters will be removed from the right.
   *
   * @return A new BasicStringView containing the trimmed string. If the input
   *         string view consists entirely of whitespace, an empty
   * BasicStringView is returned.
   */
  template<typename TChar>
  SRX_API BasicStringView<TChar> TrimRight(BasicStringView<TChar> str)
    SRX_NOEXCEPT
  {
    size_t end = str.length();
    while (end > 0 && std::isspace(str[end - 1]))
    {
      --end;
    }

    return end ? str.substr(0, end) : BasicStringView<TChar>();
  }

  /**
   * @brief Trims whitespace characters from the right end of the given string
   * view.
   *
   * @param str The string view to be trimmed. It is a view of the original
   * string from which whitespace characters will be removed from the right.
   *
   * @return A new BasicStringView containing the trimmed string. If the input
   *         string view consists entirely of whitespace, an empty
   * BasicStringView is returned.
   */
  template<typename TChar>
  SRX_API SRX_INLINE BasicStringView<TChar> TrimRight(
    const BasicString<TChar>& str) SRX_NOEXCEPT
  {
    return TrimRight(BasicStringView<TChar>(str));
  }

  /**
   * @brief Trims whitespace from both ends of the given string.
   *
   * This function removes leading and trailing whitespace characters from the
   * input string.
   *
   * @tparam TChar The character type of the string.
   * @param str The input string to be trimmed.
   * @return A BasicStringView containing the trimmed version of the input
   * string.
   */
  template<typename TChar>
  SRX_API SRX_INLINE BasicStringView<TChar> Trim(const BasicString<TChar>& str)
    SRX_NOEXCEPT
  {
    return TrimRight(TrimLeft(BasicStringView<TChar>(str)));
  }

  template<typename TChar>
  BasicStringView<TChar> TrimLeft(BasicStringView<TChar> str,
                                  const TChar            sym) SRX_NOEXCEPT
  {
    for (size_t i = 0; i < str.length(); ++i)
      if (str[i] != sym)
        return str.substr(i);

    return BasicStringView<TChar>();
  }

  template<typename TChar>
  SRX_INLINE BasicStringView<TChar> TrimLeft(const BasicString<TChar>& str,
                                             const TChar sym) SRX_NOEXCEPT
  {
    return TrimLeft(BasicStringView<TChar>(str), sym);
  }

  template<typename TChar>
  BasicStringView<TChar> TrimRight(BasicStringView<TChar> str,
                                   const TChar            sym) SRX_NOEXCEPT
  {
    for (int32 i = static_cast<int32>(str.length() - 1); i >= 0; --i)
      if (str[i] != sym)
        return str.substr(0, i + 1);

    return BasicStringView<TChar>();
  }

  template<typename TChar>
  SRX_INLINE BasicStringView<TChar> Trim(BasicStringView<TChar> str,
                                         const TChar sym) SRX_NOEXCEPT
  {
    return TrimRight(TrimLeft(str, sym), sym);
  }

  template<typename TChar>
  SRX_INLINE BasicStringView<TChar> Trim(const BasicString<TChar>& str,
                                         const TChar sym) SRX_NOEXCEPT
  {
    return TrimRight(TrimLeft(BasicStringView<TChar>(str), sym), sym);
  }

  SRX_API SRX_INLINE bool CompareNoCase(const StringView& lhs,
                                        const StringView& rhs) SRX_NOEXCEPT
  {
#ifdef _MSC_VER
    return _stricmp(lhs.data(), rhs.data()) == 0;
#else
    return strcasecmp(lhs.data(), rhs.data()) == 0;
#endif  // _MSC_VER
  }

  SRX_API SRX_INLINE bool CompareNoCase(const WStringView& lhs,
                                        const WStringView& rhs) SRX_NOEXCEPT
  {
#ifdef _MSC_VER
    return wcscmp(lhs.data(), rhs.data()) == 0;
#else
    return wcscasecmp(lhs.data(), rhs.data()) == 0;
#endif
  }

  SRX_API String ToUtf8String(WStringView wstr) SRX_NOEXCEPT;

}  // namespace
