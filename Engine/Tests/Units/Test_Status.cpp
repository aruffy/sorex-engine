#include <gtest/gtest.h>

#include <Sorex/Status.h>
#include <system_error>

using namespace Sorex;

enum class ETestErrorCode
{
  Ok = 0,
  Error
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
  // return std::error_code(static_cast<int>(errcode),
  // TStatus<TestErrorCategory>::GetErrorCategory());
  return std::error_code();
}

#define ASSERT_STATUS_OK(status)                       \
  ASSERT_TRUE((status).Ok()) << status.DebugMessage(); \
  ASSERT_TRUE((bool)(status));                         \
  ASSERT_EQ(static_cast<int>((status).GetCode()), 0)   \
    << (status).DebugMessage();                        \
  ASSERT_FALSE((status).ToString().empty()) << (status).DebugMessage()

#define ASSERT_STATUS_ERRCODE(status, errcode) \
  ASSERT_FALSE(status.Ok());                   \
  ASSERT_FALSE((bool)status);                  \
  ASSERT_EQ(status.GetCode(), (int)errcode);   \
  ASSERT_FALSE(status.ToString().empty())

TEST(Status, Main)
{
  // Ok
  Status status;
  ASSERT_STATUS_OK(status);
  ASSERT_STATUS_OK(Status::Create(EStatusCode::Ok));
  ASSERT_STATUS_OK(SRX_OK);
  /* Status statusOk(EStatusCode::Ok);
  ASSERT_STATUS_OK(statusOk);
  Status statusNoError(EStatusCode::No_Error);
  ASSERT_STATUS_OK(statusNoError);

  // Error
  for (int errcode = 1; errcode < (int)EStatusCode::Try_Again; ++errcode)
  {
    Status statusError(static_cast<EStatusCode>(errcode));
    ASSERT_STATUS_ERRCODE(statusError, static_cast<EStatusCode>(errcode));
    // Reset
    statusError.Reset();
    ASSERT_STATUS_OK(statusError);
  }

  // Custom Error
  TStatus<TestErrorCategory> testStatus;
  ASSERT_STATUS_OK(testStatus);
  TStatus<TestErrorCategory> errorStatus(ETestErrorCode::Error);
  ASSERT_STATUS_ERRCODE(errorStatus, TestErrorCategory::EStatusCode::Error);
  errorStatus.Reset();
  ASSERT_STATUS_OK(errorStatus); */
}
