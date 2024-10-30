#include <gtest/gtest.h>

#include <Sorex/Status.h>

using namespace Sorex;

#define ACCERT_STATUS_OK(status) ASSERT_TRUE(status.Success())

TEST(Status, Main)
{
  Status status;
  EXPECT_EQ(status.GetCode(), Status::EStatusCode::Ok);
  ASSERT_TRUE(status.Success());
}
