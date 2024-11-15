#pragma once

#include <random>

#include <Sorex/Types.h>
#include <Sorex/Platform.h>
#include <Sorex/Assert.h>

#ifndef M_PI
#  define M_PI 3.14159265358979323846
#endif

namespace Sorex::Math
{
  /**
   * @brief Computes the wave value at a given time.
   *
   * @tparam T The floating-point type (float or double).
   * @param t The time at which to evaluate the wave.
   * @param ampl The amplitude of the wave.
   * @param speed The speed of the wave propagation.
   * @param length The wavelength of the wave.
   *
   * @return The computed wave value at time t.
   */
  template<typename T>
  SRX_API SRX_INLINE SRX_TYPENAME
    std::enable_if_t<std::is_floating_point_v<T>, T>
    Wave(const T t, const T ampl, T speed, T length) SRX_NOEXCEPT
  {
    if constexpr (std::is_same_v<T, float>)
      return ampl * sinf(2 * (float)M_PI * speed * t / length);

    return ampl * sin(2 * T{ M_PI } * speed * t / length);
  }

  /**
   * @brief Computes the wave value at a given time.
   *
   * This function calculates the wave value based on the provided time,
   * amplitude, and period.
   *
   * @tparam T The type of the input parameters, which must be a floating point
   * type.
   *
   * @param t     The time at which to evaluate the wave function.
   * @param ampl  The amplitude of the wave.
   * @param period The period of the wave, must be non-zero.
   *
   * @return The computed wave value at the specified time.
   */
  template<typename T>
  SRX_API SRX_INLINE SRX_TYPENAME
    std::enable_if_t<std::is_floating_point_v<T>, T>
    Wave(const T t, const T ampl, const T period) SRX_NOEXCEPT
  {
    SRX_ASSERT(period != 0);

    if constexpr (std::is_same_v<T, float>)
      return ampl * sinf(2 * (float)M_PI * t / period);

    return ampl * sin(2 * T{ M_PI } * t / period);
  }

  /**
   * @brief Converts an angle from degrees to radians.
   *
   * This function takes an angle in degrees and converts it to radians using
   * the formula: radians = degrees * (π / 180).
   *
   * @note This function uses a static assertion to ensure that the template
   * type T is a floating-point type.
   *
   * @tparam T The type of the input angle, which must be a floating-point type.
   * @param deg The angle in degrees to be converted to radians.
   *
   * @return The angle in radians.
   */
  template<typename T>
  SRX_API SRX_INLINE SRX_TYPENAME
    std::enable_if_t<std::is_floating_point_v<T>, T>
    Radians(const T deg) SRX_NOEXCEPT
  {
    constexpr T kFactor = ((T)M_PI / T{ 180.f });
    return deg * kFactor;
  }

  /**
   * @brief Converts radians to degrees.
   *
   * This function takes an angle in radians and converts it to degrees using
   * the formula: degrees = radians * (180 / pi).
   *
   * @note This function uses a static assertion to ensure that the template
   * type T is a floating-point type.
   *
   * @tparam T The type of the input angle, which must be a floating-point type.
   * @param rad The angle in radians to be converted to degrees.
   *
   * @return The angle in degrees.
   */
  template<typename T>
  SRX_API SRX_INLINE SRX_TYPENAME
    std::enable_if_t<std::is_floating_point_v<T>, T>
    Degrees(const T rad) SRX_NOEXCEPT
  {
    constexpr T kFactor = (T{ 180 } / (T)M_PI);
    return rad * kFactor;
  }
  /**
   * @brief Generates a random scalar value in the range [0, 1].
   *
   * This function uses the standard library's rand() function to produce a
   * random number and normalizes it by dividing by RAND_MAX to ensure the
   * result is within the desired range.
   *
   * @return A random scalar value between 0 and 1.
   */
  SRX_API SRX_INLINE scalar_t Random() SRX_NOEXCEPT
  {
    const scalar_t r = rand();
    return r / scalar_t{ RAND_MAX };
  }

  template<typename T>
    requires std::is_integral_v<T>
  SRX_API SRX_INLINE T Random(const T max) SRX_NOEXCEPT
  {
    static_assert(std::is_same_v<T, bool> == false, "invalid integer type");

    if (max == T{ 0 })
      return T{ 0 };

    const T r = rand();
    return r % max;
  }


  template<typename T>
    requires std::is_floating_point_v<T>
  SRX_API SRX_INLINE T Alpha(const T val, const T min, const T max) SRX_NOEXCEPT
  {
    return (val - min) / (max - min);
  }

  template<typename T>
    requires std::is_floating_point_v<T>
  SRX_API SRX_INLINE T Lerp(const T v1, const T v2, const T alpha) SRX_NOEXCEPT
  {
    return v1 + (v2 - v1) * alpha;
  }

  template<typename T>
    requires std::is_floating_point_v<T> || std::is_integral_v<T>
  SRX_API SRX_INLINE T Clamp(const T val, const T min, const T max) SRX_NOEXCEPT
  {
    return (val < min) ? min : ((val > max) ? max : val);
  }

  /**
   * @brief Compares two floating-point values for equality within a specified
   * tolerance.
   *
   * This function checks if the absolute difference between two values is less
   * than or equal to a small epsilon value, or if the difference is smaller
   * than the minimum representable value for the type.
   *
   * @tparam T The type of the values being compared. Must be a floating-point
   * type.
   *
   * @param left The first value to compare.
   * @param right The second value to compare.
   *
   * @return Returns true if the two values are considered equal, false
   * otherwise.
   */
  template<class T>
  SRX_API constexpr SRX_TYPENAME
    std::enable_if_t<!std::numeric_limits<T>::is_integer, bool>
    IsEqual(const T left, const T right) SRX_NOEXCEPT
  {
    const T     diff = std::fabs(left - right);
    constexpr T kMin = std::numeric_limits<T>::min();
    return (diff <= std::numeric_limits<T>::epsilon() * std::fabs(left + right)
            || diff < kMin);
  }
}  // namespace
