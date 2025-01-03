#pragma once

#include "SxPlatform.h"

#define SRX_ENUM_BITMASK __SorexEnumAsBitmask

namespace Sorex::Details
{
  /**
   * @brief Enable bitmask support for enum classes.
   * @tparam Type The enum type for which to enable bitmask support.
   */
  template<typename Type, typename = void>
  struct SRX_API TEnumAsBitmask: std::false_type
  {};

  /*! @copydoc TEnumAsBitmask */
  template<typename Type>
  struct SRX_API
    TEnumAsBitmask<Type, std::void_t<decltype(Type::SRX_ENUM_BITMASK)>>
    : std::is_enum<Type>
  {};

  /**
   * @brief Helper variable template.
   * @tparam Type The enum class type for which to enable bitmask support.
   */
  template<typename Type>
  constexpr bool enum_as_bitmask_v = TEnumAsBitmask<Type>::value;
}  // namespace

/**
 * @brief Operator available for enums for which bitmask support is enabled.
 * @tparam Type Enum class type.
 * @param lhs The first value to use.
 * @param rhs The second value to use.
 * @return The result of invoking the operator on the underlying types of the
 * two values provided.
 */
template<typename Type>
SRX_NODISCARD constexpr std::
  enable_if_t<Sorex::Details::enum_as_bitmask_v<Type>, Type>
  operator|(const Type lhs, const Type rhs) SRX_NOEXCEPT
{
  return static_cast<Type>(static_cast<std::underlying_type_t<Type>>(lhs)
                           | static_cast<std::underlying_type_t<Type>>(rhs));
}

/*! @copydoc operator| */
template<typename Type>
SRX_NODISCARD constexpr std::
  enable_if_t<Sorex::Details::enum_as_bitmask_v<Type>, Type>
  operator&(const Type lhs, const Type rhs) SRX_NOEXCEPT
{
  return static_cast<Type>(static_cast<std::underlying_type_t<Type>>(lhs)
                           & static_cast<std::underlying_type_t<Type>>(rhs));
}

/*! @copydoc operator| */
template<typename Type>
SRX_NODISCARD constexpr std::
  enable_if_t<Sorex::Details::enum_as_bitmask_v<Type>, Type>
  operator^(const Type lhs, const Type rhs) SRX_NOEXCEPT
{
  return static_cast<Type>(static_cast<std::underlying_type_t<Type>>(lhs)
                           ^ static_cast<std::underlying_type_t<Type>>(rhs));
}

/**
 * @brief Operator available for enums for which bitmask support is enabled.
 * @tparam Type Enum class type.
 * @param value The value to use.
 * @return The result of invoking the operator on the underlying types of the
 * value provided.
 */
template<typename Type>
SRX_NODISCARD constexpr std::
  enable_if_t<Sorex::Details::enum_as_bitmask_v<Type>, Type>
  operator~(const Type value) SRX_NOEXCEPT
{
  return static_cast<Type>(~static_cast<std::underlying_type_t<Type>>(value));
}

/*! @copydoc operator~ */
template<typename Type>
SRX_NODISCARD constexpr std::
  enable_if_t<Sorex::Details::enum_as_bitmask_v<Type>, bool>
  operator!(const Type value) SRX_NOEXCEPT
{
  return !static_cast<std::underlying_type_t<Type>>(value);
}

/*! @copydoc operator| */
template<typename Type>
constexpr std::enable_if_t<Sorex::Details::enum_as_bitmask_v<Type>, Type&>
operator|=(Type& lhs, const Type rhs) SRX_NOEXCEPT
{
  return (lhs = (lhs | rhs));
}

/*! @copydoc operator| */
template<typename Type>
constexpr std::enable_if_t<Sorex::Details::enum_as_bitmask_v<Type>, Type&>
operator&=(Type& lhs, const Type rhs) SRX_NOEXCEPT
{
  return (lhs = (lhs & rhs));
}

/*! @copydoc operator| */
template<typename Type>
constexpr std::enable_if_t<Sorex::Details::enum_as_bitmask_v<Type>, Type&>
operator^=(Type& lhs, const Type rhs) SRX_NOEXCEPT
{
  return (lhs = (lhs ^ rhs));
}

namespace Sorex::Utils
{
  template<typename Type>
  SRX_NODISCARD SRX_INLINE constexpr std::
    enable_if_t<Sorex::Details::enum_as_bitmask_v<Type>, bool>
    CheckBitmask(const Type value, const Type bitmask) SRX_NOEXCEPT
  {
    return (value & bitmask) == bitmask;
  }
}
