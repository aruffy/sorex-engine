#include <gtest/gtest.h>

#include <Sorex/Utils/BytesUtils.h>

using namespace Sorex;

template<typename T>
using Pair = TPair<T, T>;

TEST(BytesUtils, Swap)
{
  Pair<uint8>  p_u8   = std::make_pair(0b11010001, 0b10001011);
  Pair<int8>   p_i8   = std::make_pair(0b00010101, 0b10101000);
  Pair<uint16> p_u16  = std::make_pair(0b0101010101010111, 0b1110101010101010);
  Pair<uint16> p2_u16 = std::make_pair(0b1011001011110000, 0b0000111101001101);

  EXPECT_EQ(Utils::SwapBits(p_u8.first), p_u8.second);
  EXPECT_EQ(Utils::SwapBits(p_i8.first), p_i8.second);
  EXPECT_EQ(Utils::SwapBits(p_u16.first), p_u16.second);
  EXPECT_EQ(Utils::SwapBits(p2_u16.first), p2_u16.second);

  const TArray<Pair<uint16>, 8> ap_u16 = {
    std::make_pair(0x0011, 0x1100), std::make_pair(0x1122, 0x2211),
    std::make_pair(0xFFFF, 0xFFFF), std::make_pair(0x0000, 0x0000),
    std::make_pair(0x1001, 0x0110), std::make_pair(0x1234, 0x3412),
    std::make_pair(0xbeaf, 0xafbe), std::make_pair(0xceed, 0xedce)
  };

  for (const Pair<uint16>& p : ap_u16)
  {
    EXPECT_EQ(Utils::SwapBytes(p.first), p.second);
    EXPECT_EQ(Utils::SwapBytes<int16>(int16(p.first)), int16(p.second));
  }

  const TArray<Pair<uint32>, 8> ap_u32 = {
    std::make_pair(0x00112233, 0x33221100),
    std::make_pair(0x11223344, 0x44332211),
    std::make_pair(0x00, 0x00),
    std::make_pair(0xffff0000, 0xffff),
    std::make_pair(0xffffffff, 0xffffffff),
    std::make_pair(0xbeafbeaf, 0xafbeafbe),
    std::make_pair(0x10012002, 0x02200110),
    std::make_pair(0x000ff000, 0x00f00f00)
  };

  for (const Pair<uint32>& p : ap_u32)
  {
    EXPECT_EQ(Utils::SwapBytes(p.first), p.second);
    EXPECT_EQ(Utils::SwapBytes<int32>((int32)p.first), (int32)p.second);
  }

  EXPECT_EQ(Utils::SwapBytes(uint64(0x1122334455667788)), 0x8877665544332211);
}
