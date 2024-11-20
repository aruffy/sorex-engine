#include <gtest/gtest.h>

#include <iostream>
#include <vector>
#include <limits>

#include <Sorex/Math/Maths.h>

using namespace Sorex;

struct TestData
{
  float lhs;
  float rhs;
  bool  result;
};

TEST(Math, IsEqual)
{
  constexpr float       epsilon     = std::numeric_limits<float>::epsilon();
  constexpr float       kMin        = std::numeric_limits<float>::min();
  std::vector<TestData> testDataset = {
    { 1.0f, 1.0f, true },
    { 1.0f, 1.0f + epsilon / 2, true },
    { 1.0f, 1.0f + epsilon, true },
    { 1.0f, 1.0f + epsilon * 2, false },
    { 0.1f, 0.1f, true },
    { 0.1f, 0.1f + epsilon, false },
    { 1.0e10f, 1.0e10f + epsilon * 1.0e10f, true },
    { 1.0e10f, 1.0e10f + epsilon * 1.0e10f * 2, false },
    { -1.0f, -1.0f, true },
    { -1.0f, -1.0f - epsilon / 2, true },
    { -1.0f, -1.0f - epsilon, true },
    { -1.0f, -1.0f - epsilon * 2, false },
    { 0.0f, 0.0f, true },
    { 0.0f, kMin, false },
    { kMin, 0.0f, false },
    { 0.0f, -kMin, false },
    { kMin, kMin * 2, false },
    { kMin, kMin / 2, true },
    { 1.0f, 1.1f, false },
    { 1.0f, 0.9f, false },
    { 1e-6f, 1e-6f, true },
    { 1e-6f, 1e-6f + epsilon, false },
    { 1e-6f, 1e-6f + epsilon * 2, false },
    { 1.0f, -1.0f, false },
    { -1.0f, 1.0f, false },
    { -100000.f, -100000.f, true },
    { -100000.1f, -100000.f, false }
  };

  for (const auto& data : testDataset)
  {
    ASSERT_EQ(Math::IsEqual(data.lhs, data.rhs), data.result)
      << "Value: '" << data.lhs << "' " << (data.result ? "isn't" : "is")
      << " equal '" << data.rhs << "' diff='" << std::fabs(data.lhs - data.rhs)
      << "' epsd='" << (std::fabs(data.lhs + data.rhs) * epsilon * 0.8f) << "'";
    ASSERT_EQ(Math::IsEqual(data.rhs, data.lhs), data.result);
  }
}
