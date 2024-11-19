#include <gtest/gtest.h>

#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include <glm.hpp>

#include <Sorex/Math/Matrix3x3.h>
#include <Sorex/Math/Maths.h>

using namespace Sorex;

namespace
{
  Vec3 GetVec3(const glm::mat4& m, size_t i)
  {
    const auto v = m[i];
    return Vec3(v.x, v.y, v.w);
  }

  inline Vec3 ToVec3(const glm::vec3& vec)
  {
    return Vec3(vec.x, vec.y, vec.z);
  }

  const std::vector<float> kAngles = {
    0.f,   10.f,  15.f,  30.f,  45.f,   60.f,      90.f,       105.f,
    120.f, 180.f, 270.f, 360.f, 0.001f, 34.12454f, 89.988312f, 159.99991f
  };
}

#define EXPECT_EQ_VEC3(v1, v2)                                             \
  EXPECT_EQ((v1), (v2)) << "Vec3: '" << v1.x << '/' << v1.y << '/' << v1.z \
                        << "' ins't equal '" << v2.x << '/' << v2.y << '/' \
                        << v2.z << '\'';

#define EXPECT_EQ_MAT4(mat3, mat4)           \
  EXPECT_EQ_VEC3(mat3[0], GetVec3(mat4, 0)); \
  EXPECT_EQ_VEC3(mat3[1], GetVec3(mat4, 1)); \
  EXPECT_EQ_VEC3(mat3[2], GetVec3(mat4, 3))

#define EXPECT_EQ_MAT3(mat3, glm_mat3)          \
  EXPECT_EQ_VEC3(mat3[0], ToVec3(glm_mat3[0])); \
  EXPECT_EQ_VEC3(mat3[1], ToVec3(glm_mat3[1])); \
  EXPECT_EQ_VEC3(mat3[2], ToVec3(glm_mat3[2]))

TEST(Matrix3x3, Main)
{
  Mat3 mat3;
  for (float val : mat3)
    EXPECT_EQ(val, 0.f);

  for (auto it = mat3.cbegin(); it != mat3.cend(); ++it)
    EXPECT_EQ(*it, 0.f);

  const Mat3 identify = Mat3::Identity();
  EXPECT_EQ(identify[0], Vec3(1.f, 0.f, 0.f));
  EXPECT_EQ(identify[1], Vec3(0.f, 1.f, 0.f));
  EXPECT_EQ(identify[2], Vec3(0.f, 0.f, 1.f));

  mat3 = Mat3(1.f);
  for (size_t i = 0; i < 3; ++i)
    EXPECT_EQ(mat3[i], identify[i]);

  mat3.Fill(3.f);
  for (float val : mat3)
    EXPECT_EQ(val, 3.f);

  Mat3 m;
  m = mat3;
  for (size_t i = 0; i < 3; ++i)
    for (size_t j = 0; j < 3; ++j)
      EXPECT_EQ(m.Get(i, j), mat3.Get(i, j));

  mat3 = Mat3(identify.GetValuePtr());
  for (size_t i = 0; i < 3; ++i)
    EXPECT_EQ(mat3[i], identify[i]);
}

TEST(Matrix3x3, Translation)
{
  const Vec2 vtrans(2.f, 3.f);
  Mat3       m1 = Mat3::Translation(vtrans.x, vtrans.y);
  EXPECT_EQ(m1[0], Vec3(1.f, 0.f, 0.f));
  EXPECT_EQ(m1[1], Vec3(0.f, 1.f, 0.f));
  EXPECT_EQ(m1[2], Vec3(vtrans.x, vtrans.y, 1.f));

  m1.SetIdentity();
  EXPECT_EQ(m1[0], Vec3(1.f, 0.f, 0.f));
  EXPECT_EQ(m1[1], Vec3(0.f, 1.f, 0.f));
  EXPECT_EQ(m1[2], Vec3(0.f, 0.f, 1.f));

  m1.Translate(vtrans);
  EXPECT_EQ(m1[0], Vec3(1.f, 0.f, 0.f));
  EXPECT_EQ(m1[1], Vec3(0.f, 1.f, 0.f));
  EXPECT_EQ(m1[2], Vec3(vtrans.x, vtrans.y, 1.f));

  auto m2 = Mat3::Translate(Mat3::Identity(), vtrans);
  EXPECT_EQ(m2[0], Vec3(1.f, 0.f, 0.f));
  EXPECT_EQ(m2[1], Vec3(0.f, 1.f, 0.f));
  EXPECT_EQ(m2[2], Vec3(vtrans.x, vtrans.y, 1.f));

  glm::mat4  m(1.f);
  const bool bTheSame =
    std::is_same<glm::mat4::value_type, Mat3::value_type>::value;
  ASSERT_TRUE(bTheSame);

  EXPECT_EQ(GetVec3(m, 0), m2[0]);
  EXPECT_EQ(GetVec3(m, 1), m2[1]);
  EXPECT_EQ(GetVec3(m, 1), m2[1]);

  m      = glm::translate(m, glm::vec3(vtrans.x, vtrans.y, 0.f));
  Vec3 v = GetVec3(m, 3);
  EXPECT_EQ(m2[2].x, v.x);
  EXPECT_EQ(m2[2].y, v.y);

  m2.Translate(Vec2());
  m = glm::translate(m, glm::vec3(0.f, 0.f, 0.f));
  v = GetVec3(m, 3);
  EXPECT_EQ(m2[2].x, v.x);
  EXPECT_EQ(m2[2].y, v.y);
}

TEST(Matrix3x3, Rotation)
{
  Mat3      m3(1.f);
  glm::mat4 m4(1.f);
  EXPECT_EQ_MAT4(m3, m4);

  for (float d : kAngles)
  {
    ASSERT_EQ(Math::Radians(d), glm::radians(d));

    Mat3      tmp1 = Mat3::Rotation(Math::Radians(d));
    glm::mat4 tmp2 =
      glm::rotate(glm::mat4(1.f), glm::radians(d), glm::vec3(0.f, 0.f, 1.f));
    EXPECT_EQ_MAT4(tmp1, tmp2);

    m3.Rotate(Math::Radians(d));
    m4 = glm::rotate(m4, glm::radians(d), glm::vec3(0.f, 0.f, 1.f));
    EXPECT_EQ_MAT4(m3, m4);
  }
}

TEST(Matrix3x3, Common)
{
  Mat3      m3(1.f);
  glm::mat4 m4(1.f);
  EXPECT_EQ_MAT4(m3, m4);

  for (size_t i = 0; i < kAngles.size(); ++i)
  {
    if (kAngles[i] == 0.f)
      continue;

    const float scaleX =
      (i && kAngles[i - 1]) ? kAngles[i] / kAngles[i - 1] : kAngles[i];
    const float scaleY   = (i % 2) ? scaleX / 1.8f : scaleX * 2.1f;
    const float rotation = kAngles[i];
    const Vec2  translation(scaleX * rotation / 4.f, scaleY * rotation / 3.f);

    m3.Translate(translation);
    m4 = glm::translate(m4, glm::vec3(translation.x, translation.y, 0.f));
    EXPECT_EQ_MAT4(m3, m4);

    m3.Scale(Vec2(scaleX, scaleY));
    m4 = glm::scale(m4, glm::vec3(scaleX, scaleY, 0.f));
    EXPECT_EQ_MAT4(m3, m4);

    m3.Rotate(Math::Radians(rotation));
    m4 = glm::rotate(m4, glm::radians(rotation), glm::vec3(0.f, 0.f, 1.f));
    EXPECT_EQ_MAT4(m3, m4);

    glm::mat4 tm4(1.f);
    tm4 = glm::translate(tm4, glm::vec3(translation.x, translation.y, 0.f));
    tm4 = glm::rotate(tm4, glm::radians(rotation), glm::vec3(0.f, 0.f, 1.f));
    tm4 = glm::scale(tm4, glm::vec3(scaleX, 5.f, 1.f));
    Mat3 tm3 =
      Mat3::Create(translation, Math::Radians(rotation), Vec2(scaleX, 5.f));
    EXPECT_EQ_MAT4(tm3, tm4);


    glm::mat4 mm4 = m4 * tm4;
    Mat3      mm3 = m3 * tm3;
    EXPECT_EQ_MAT4(mm3, mm4);

    glm::mat4 sm4 = m4 + tm4;
    Mat3      sm3 = m3 + tm3;
    EXPECT_EQ_MAT4(sm3, sm4);

    sm4 = m4 - tm4;
    sm3 = m3 - tm3;
    EXPECT_EQ_MAT4(sm3, sm4);

    glm::mat3 igm3;
    auto      v = GetVec3(m4, 0);
    igm3[0]     = glm::vec3(v.x, v.y, v.z);
    v           = GetVec3(m4, 1);
    igm3[1]     = glm::vec3(v.x, v.y, v.z);
    v           = GetVec3(m4, 3);
    igm3[2]     = glm::vec3(v.x, v.y, v.z);

    igm3     = glm::inverse(igm3);
    Mat3 im3 = Mat3::Invert(m3);

    EXPECT_EQ_VEC3(ToVec3(igm3[0]), im3[0]);
    EXPECT_EQ_VEC3(ToVec3(igm3[1]), im3[1]);
    EXPECT_EQ_VEC3(ToVec3(igm3[2]), im3[2]);
  }
}

TEST(Matrix3x3, Myltiply)
{
  glm::mat4 m4(1.f);
  Mat3      m3(1.f);

  EXPECT_EQ_MAT4(m3, m4);

  glm::vec3   t(25.f, 80.f, 0.f);
  const float rotation = 30.f;
  glm::vec3   scale(2.1f, 1.7f, 1.f);

  m4 = glm::translate(m4, t);
  m4 = glm::rotate(m4, glm::radians(rotation), glm::vec3(0.f, 0.f, 1.f));
  m4 = glm::scale(m4, scale);

  m3.Translate(t.x, t.y);
  m3.Rotate(Math::Radians(rotation));
  m3.Scale(scale.x, scale.y);

  EXPECT_EQ_MAT4(m3, m4);

  auto mm4 = glm::translate(m4, t) * m4;
  auto mm3 = Mat3::Translate(m3, Vec2(t.x, t.y)) * m3;
  EXPECT_EQ_MAT4(mm3, mm4);

  mm3 = Mat3::Create(Vec2(t.x * 2.f, t.y / 4.f),
                     Math::Radians(rotation / 2.f),
                     Vec2(scale.y, scale.x));
  mm4 = glm::translate(glm::mat4(1.f), glm::vec3(t.x * 2.f, t.y / 4.f, 0.f));
  mm4 =
    glm::rotate(mm4, glm::radians(rotation / 2.f), glm::vec3(0.f, 0.f, 1.f));
  mm4 = glm::scale(mm4, glm::vec3(scale.y, scale.x, 1.f));

  EXPECT_EQ_MAT4(mm3, mm4);
}
