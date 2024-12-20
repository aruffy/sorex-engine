#include <gtest/gtest.h>

#include <Sorex/SxStatus.h>
#include <system_error>

using namespace Sorex;

enum class ETestErrorCode
{
  Test_Ok = 0,
  Test_Error
};

namespace std
{
  template<>
  struct is_error_code_enum<ETestErrorCode>: public true_type
  {};
}

class TestErrorCategory final: public std::error_category
{
  public:
  using EStatusCode = ETestErrorCode;

  const char* name() const noexcept override { return "TestErrorCategory"; }
  std::string message(int errcode) const override
  {
    return errcode ? "Error" : "Ok";
  }
};

inline std::error_code make_error_code(ETestErrorCode errcode)
{
  static const TestErrorCategory errorCategory;
  return std::error_code(static_cast<int>(errcode), errorCategory);
}

#define ASSERT_STATUS_OK(status)                                             \
  ASSERT_TRUE((status).Ok()) << status.ToString();                           \
  ASSERT_TRUE((bool)(status));                                               \
  ASSERT_EQ(static_cast<int>((status).GetCode()), 0) << (status).ToString(); \
  ASSERT_FALSE((status).ToString().empty()) << (status).ToString()

#define ASSERT_STATUS_ERRCODE(status, errcode)        \
  ASSERT_FALSE((status).Ok()) << (status).ToString(); \
  ASSERT_FALSE((bool)(status));                       \
  ASSERT_EQ((status).GetCode(), (int)(errcode));      \
  ASSERT_FALSE((status).ToString().empty())

TEST(Status, Main)
{
  // Ok
  Status status;
  Status defStatusOk(EStatusCode::Ok);
  ASSERT_STATUS_OK(status);
  ASSERT_STATUS_OK(defStatusOk);
  ASSERT_STATUS_OK(Status::Create(EStatusCode::Ok));
  ASSERT_STATUS_OK(Status::Create(EStatusCode::No_Error));
  ASSERT_STATUS_OK(SRX_OK);
  ASSERT_STATUS_OK(SRX_STATUS_MSG(EStatusCode::Ok, "Hello"));
  ASSERT_STATUS_OK(SRX_STATUS_MSG(EStatusCode::Ok, "Hello {}", "world!"));

  // Error
  for (int errcode = 1; errcode < (int)EStatusCode::Try_Again; ++errcode)
  {
    Status statusError = Status::Create(static_cast<EStatusCode>(errcode));
    ASSERT_STATUS_ERRCODE(statusError, static_cast<EStatusCode>(errcode));
    ASSERT_STATUS_ERRCODE(
      SRX_STATUS_MSG(static_cast<EStatusCode>(errcode), "Error"),
      static_cast<EStatusCode>(errcode));
    ASSERT_STATUS_ERRCODE(
      SRX_STATUS_MSG(static_cast<EStatusCode>(errcode), "Error {}", errcode),
      static_cast<EStatusCode>(errcode));
    // Reset
    statusError.Reset();
    ASSERT_STATUS_OK(statusError);
  }

  // Custom Error
  Status testOk = Status::Create(ETestErrorCode::Test_Ok);
  ASSERT_STATUS_OK(testOk);
  Status errorStatus = Status::Create(ETestErrorCode::Test_Error);
  ASSERT_STATUS_ERRCODE(errorStatus,
                        TestErrorCategory::EStatusCode::Test_Error);
  errorStatus.Reset();
  ASSERT_STATUS_OK(errorStatus);
  Status testError(ETestErrorCode::Test_Error);
  ASSERT_STATUS_ERRCODE(testError, ETestErrorCode::Test_Error);
  Status statusMsg = SRX_STATUS_MSG(ETestErrorCode::Test_Ok, "Hello");
  ASSERT_STATUS_ERRCODE(SRX_STATUS_MSG(ETestErrorCode::Test_Error, "Error"),
                        ETestErrorCode::Test_Error);
  ASSERT_STATUS_OK(statusMsg);
  EXPECT_EQ(statusMsg.GetCategory().name(), TestErrorCategory().name());
}
