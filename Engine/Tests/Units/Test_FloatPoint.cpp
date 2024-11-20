#include <gtest/gtest.h>

#include <Sorex/Math/Point.h>

using namespace Sorex;

#define RFY_CHECK_POINT(p1, ax, ay) \
  EXPECT_EQ(p1.x, ax);              \
  EXPECT_EQ(p1.y, ay);              \
  EXPECT_EQ(p1.data[0], ax);        \
  EXPECT_EQ(p1.data[1], ay)

#define RFY_POINTS_EQ(p1, p2)        \
  EXPECT_EQ(p1.x, p2.x);             \
  EXPECT_EQ(p1.y, p2.y);             \
  EXPECT_EQ(p1.data[0], p2.x);       \
  EXPECT_EQ(p1.data[1], p2.y);       \
  EXPECT_EQ(p1.data[0], p2.data[0]); \
  EXPECT_EQ(p1.data[1], p2.data[1]); \
  EXPECT_TRUE(p1 == p2);             \
  EXPECT_TRUE((p1).IsEqual(p2));     \
  EXPECT_TRUE(p1.IsEqual(p2))

#define RFY_POINTS_NE(p1, p2)                                        \
  EXPECT_TRUE(p1.x != p2.x || p1.y != p2.y);                         \
  EXPECT_TRUE(p1.data[0] != p2.data[0] || p1.data[1] != p2.data[1]); \
  EXPECT_TRUE(p1 != p2);                                             \
  EXPECT_FALSE(p1 == p2);                                            \
  EXPECT_FALSE((p1).IsEqual(p2));                                    \
  EXPECT_FALSE(p1.IsEqual(p2))

TEST(FloatingPoint, Main)
{
  EXPECT_EQ(sizeof(Point), 2 * sizeof(float));
  EXPECT_EQ(sizeof(Math::TFloatingPoint<double>), 2 * sizeof(double));

  Point p0;
  RFY_CHECK_POINT(p0, 0.f, 0.f);

  const Point p1(1.1f, 2.3f);
  RFY_CHECK_POINT(p1, 1.1f, 2.3f);
  RFY_POINTS_NE(p1, p0);

  Point p2(p1);
  RFY_CHECK_POINT(p2, 1.1f, 2.3f);
  RFY_POINTS_EQ(p2, p1);

  p2 = p0;
  RFY_CHECK_POINT(p2, 0.f, 0.f);
  RFY_POINTS_EQ(p2, p0);

  Point p3 = p1;
  RFY_CHECK_POINT(p3, 1.1f, 2.3f);
  RFY_POINTS_EQ(p3, p1);

  Point p4(12.34f, 343.532f);
  RFY_CHECK_POINT(p4, 12.34f, 343.532f);
  RFY_POINTS_NE(p4, p3);

  p4.Swap(p3);
  RFY_POINTS_NE(p3, p4);
  RFY_CHECK_POINT(p3, 12.34f, 343.532f);
  RFY_CHECK_POINT(p4, 1.1f, 2.3f);

  Point p5;
  Point::Swap(p5, p4);
  RFY_POINTS_NE(p5, p4);
  RFY_CHECK_POINT(p4, 0, 0);
  RFY_CHECK_POINT(p5, 1.1f, 2.3f);

  Point p6 = -p1;
  RFY_CHECK_POINT(p6, -1.1f, -2.3f);

  p6 += p1;
  RFY_CHECK_POINT(p6, 0.f, 0.f);

  p6 -= Point(1.1f, 2.2f);
  RFY_CHECK_POINT(p6, -1.1f, -2.2f);

  Point p10(p5 + p1);
  RFY_CHECK_POINT(p10, (p5.x + p1.x), (p5.y + p1.y));

  Point p7(p3 - p10);
  RFY_CHECK_POINT(p7, (p3.x - p10.x), (p3.y - p10.y));

  constexpr float m1 = 3.f;
  Point           p8 = p10 * m1;
  RFY_CHECK_POINT(p8, (p10.x * m1), (p10.y * m1));

  Point p9 = p7;
  RFY_POINTS_EQ(p9, p7);

  float m = 5.1f;
  p9 *= m;
  RFY_CHECK_POINT(p9, (p7.x * m), (p7.y * m));
  p9 /= m;
  RFY_POINTS_EQ(p9, p7);

  TArray<Point, 4>                        points  = { Point(2.3f, -2.3f),
                                                      Point(3.8f, -3.8f),
                                                      Point(5.5f, -5.5f),
                                                      Point(0.1f, 99.9f) };
  TArray<Math::TFloatingPoint<double>, 4> dpoints = {
    Math::TFloatingPoint<double>(2.3, -2.3),
    Math::TFloatingPoint<double>(3.8, -3.8),
    Math::TFloatingPoint<double>(5.5, -5.5),
    Math::TFloatingPoint<double>(0.1, 99.9)
  };

  TArray<PointInt, 4> trunc = { PointInt(2, -2),
                                PointInt(3, -3),
                                PointInt(5, -5),
                                PointInt(0, 99) };

  ASSERT_EQ(points.size(), dpoints.size());
  ASSERT_EQ(points.size(), trunc.size());

  Point                        point;
  Math::TFloatingPoint<double> dpoint;
  PointInt                     p_i32;
  for (size_t i = 0; i < points.size(); ++i)
  {
    points[i].Trunc(p_i32);
    RFY_CHECK_POINT(trunc[i], p_i32.x, p_i32.y);
    point = points[i];
    point.Trunc();
    RFY_CHECK_POINT(point, (float)p_i32.x, (float)p_i32.y);

    p_i32.x = p_i32.y = 0;
    dpoints[i].Trunc(p_i32);
    RFY_CHECK_POINT(trunc[i], p_i32.x, p_i32.y);
    dpoint = dpoints[i];
    dpoint.Trunc();
    RFY_CHECK_POINT(dpoint, (float)p_i32.x, (float)p_i32.y);
  }

  TArray<PointInt, 4> floor = { PointInt(2, -3),
                                PointInt(3, -4),
                                PointInt(5, -6),
                                PointInt(0, 99) };

  ASSERT_EQ(points.size(), floor.size());
  for (size_t i = 0; i < points.size(); ++i)
  {
    points[i].Floor(p_i32);
    RFY_CHECK_POINT(floor[i], p_i32.x, p_i32.y);
    point = points[i];
    point.Floor();
    RFY_CHECK_POINT(point, (float)p_i32.x, (float)p_i32.y);

    p_i32.x = p_i32.y = 0;
    dpoints[i].Floor(p_i32);
    RFY_CHECK_POINT(floor[i], p_i32.x, p_i32.y);
    dpoint = dpoints[i];
    dpoint.Floor();
    RFY_CHECK_POINT(dpoint, (float)p_i32.x, (float)p_i32.y);
  }

  TArray<PointInt, 4> round = { PointInt(2, -2),
                                PointInt(4, -4),
                                PointInt(6, -6),
                                PointInt(0, 100) };

  ASSERT_EQ(points.size(), round.size());
  for (size_t i = 0; i < points.size(); ++i)
  {
    points[i].Round(p_i32);
    RFY_CHECK_POINT(round[i], p_i32.x, p_i32.y);
    point = points[i];
    point.Round();
    RFY_CHECK_POINT(point, (float)p_i32.x, (float)p_i32.y);

    p_i32.x = p_i32.y = 0;
    dpoints[i].Round(p_i32);
    RFY_CHECK_POINT(round[i], p_i32.x, p_i32.y);
    dpoint = dpoints[i];
    dpoint.Round();
    RFY_CHECK_POINT(dpoint, (float)p_i32.x, (float)p_i32.y);
  }

  TArray<PointInt, 4> ceil = { PointInt(3, -2),
                               PointInt(4, -3),
                               PointInt(6, -5),
                               PointInt(1, 100) };

  ASSERT_EQ(points.size(), ceil.size());
  for (size_t i = 0; i < points.size(); ++i)
  {
    points[i].Ceil(p_i32);
    RFY_CHECK_POINT(ceil[i], p_i32.x, p_i32.y);
    point = points[i];
    point.Ceil();
    RFY_CHECK_POINT(point, (float)p_i32.x, (float)p_i32.y);

    p_i32.x = p_i32.y = 0;
    dpoints[i].Ceil(p_i32);
    RFY_CHECK_POINT(ceil[i], p_i32.x, p_i32.y);
    dpoint = dpoints[i];
    dpoint.Ceil();
    RFY_CHECK_POINT(dpoint, (float)p_i32.x, (float)p_i32.y);
  }
}
