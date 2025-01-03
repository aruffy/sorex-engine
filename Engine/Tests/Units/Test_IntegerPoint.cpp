#include <gtest/gtest.h>

#include <cmath>

#include <Sorex/Math/SxPoint.h>

using namespace Sorex::Math;

typedef Sorex::Math::TIntegerPoint<std::int32_t>  PointInt32;
typedef Sorex::Math::TIntegerPoint<std::uint32_t> PointUInt32;

typedef Sorex::Math::TIntegerPoint<std::int8_t>  PointInt8;
typedef Sorex::Math::TIntegerPoint<std::uint8_t> PointUInt8;

TEST(TIntegerPointTest, DefaultConstructor)
{
  PointInt32 point;
  EXPECT_EQ(point.x, 0);
  EXPECT_EQ(point.y, 0);

  PointUInt8 pui8;
  EXPECT_EQ(pui8.x, 0);
  EXPECT_EQ(pui8.x, 0);
}

TEST(TIntegerPointTest, ParameterizedConstructor)
{
  PointInt32 point(3, -4);
  EXPECT_EQ(point.x, 3);
  EXPECT_EQ(point.y, -4);

  // PointUInt32 np(-2, -2); NOT COMPILED
  PointUInt32 pu(32U, 2U);
  EXPECT_EQ(pu.x, 32);
  EXPECT_EQ(pu.y, 2);
  std::cout << pu.y << std::endl;
}

TEST(TIntegerPointTest, CopyConstructor)
{
  PointUInt32 point1(5U, 6U);
  PointUInt32 point2(point1);
  EXPECT_EQ(point2.x, 5);
  EXPECT_EQ(point2.y, 6);
}

TEST(TIntegerPointTest, AssignmentOperator)
{
  TIntegerPoint<int> point1(7, 8);
  TIntegerPoint<int> point2;
  point2 = point1;
  EXPECT_EQ(point2.x, 7);
  EXPECT_EQ(point2.y, 8);
}

TEST(TIntegerPointTest, AdditionOperator)
{
  TIntegerPoint<int> point1(1, 2);
  TIntegerPoint<int> point2(3, 4);
  TIntegerPoint<int> result = point1 + point2;
  EXPECT_EQ(result.x, 4);
  EXPECT_EQ(result.y, 6);
}

TEST(TIntegerPointTest, SubtractionOperator)
{
  TIntegerPoint<int> point1(5, 6);
  TIntegerPoint<int> point2(3, 4);
  TIntegerPoint<int> result = point1 - point2;
  EXPECT_EQ(result.x, 2);
  EXPECT_EQ(result.y, 2);
}

#define CHECK_DISTANCE(p1, p2, distance, result)                               \
  ASSERT_DOUBLE_EQ((distance), (result))                                       \
    << "Invalid Distance: {" << (p1).x << ',' << (p1).y << "} and {" << (p2).x \
    << ',' << (p2).y << "} distance=" << (distance)

TEST(TIntegerPointTest, SwapMethod)
{
  TIntegerPoint<int> point1(1, 2);
  TIntegerPoint<int> point2(3, 4);
  point1.Swap(point2);
  EXPECT_EQ(point1.x, 3);
  EXPECT_EQ(point1.y, 4);
  EXPECT_EQ(point2.x, 1);
  EXPECT_EQ(point2.y, 2);
}

TEST(TIntegerPointTest, EqualityOperator)
{
  TIntegerPoint<int> point1(1, 2);
  TIntegerPoint<int> point2(1, 2);
  EXPECT_TRUE(point1 == point2);
}

TEST(TIntegerPointTest, InequalityOperator)
{
  TIntegerPoint<int> point1(1, 2);
  TIntegerPoint<int> point2(2, 3);
  EXPECT_TRUE(point1 != point2);
}

TEST(TIntegerPointTest, NegationOperator)
{
  TIntegerPoint<int> point(3, 4);
  TIntegerPoint<int> negated = -point;
  EXPECT_EQ(negated.x, -3);
  EXPECT_EQ(negated.y, -4);
}

TEST(TIntegerPointTest, MultiplicationOperator)
{
  TIntegerPoint<int> point(2, 3);
  TIntegerPoint<int> result = point * 2;
  EXPECT_EQ(result.x, 4);
  EXPECT_EQ(result.y, 6);
}

TEST(TIntegerPointTest, DivisionOperator)
{
  TIntegerPoint<int> point(6, 8);
  point /= 2;
  EXPECT_EQ(point.x, 3);
  EXPECT_EQ(point.y, 4);
}
