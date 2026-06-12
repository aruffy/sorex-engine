#include <gtest/gtest.h>

#include <Sorex/SxMemoryStream.h>

using namespace Sorex;

#define RONLY_STREAM_CHECK_ACCESS(__stream, __readable)       \
  EXPECT_FALSE((__stream).Check(EAccessMode::None));          \
  EXPECT_EQ((__stream).Check(EAccessMode::Read), __readable); \
  EXPECT_FALSE((__stream).Check(EAccessMode::ReadWrite));     \
  EXPECT_FALSE((__stream).Check(EAccessMode::Write))

#define RONLY_STREAM_CHECK_INVALID(__stream, __storage, __errcode)    \
  EXPECT_FALSE((__stream).IsOpen());                                  \
  EXPECT_EQ((__stream).GetSize(), 0);                                 \
  EXPECT_EQ((__stream).begin(), nullptr);                             \
  EXPECT_EQ((__stream).end(), nullptr);                               \
  EXPECT_EQ((__stream).GetData(), nullptr);                           \
  EXPECT_FALSE((__stream).Advance());                                 \
  EXPECT_FALSE((__stream).Advance(0));                                \
  EXPECT_FALSE((__stream).Advance(8));                                \
  RONLY_STREAM_CHECK_ACCESS((__stream), false);                       \
  EXPECT_EQ((__stream).GetName(), StringView());                      \
  EXPECT_EQ((__stream).GetLength(), 0);                               \
  EXPECT_EQ((__stream).GetPosition(), 0);                             \
  EXPECT_EQ((__stream).GetStatus().GetCode(), (int)(__errcode));      \
  EXPECT_FALSE((__stream).Peek(__storage[0]));                        \
  EXPECT_FALSE((__stream).Seek(0, ESeekMode::Begin));                 \
  EXPECT_FALSE((__stream).Seek(0, ESeekMode::Current));               \
  EXPECT_FALSE((__stream).Seek(0, ESeekMode::End));                   \
  EXPECT_EQ((__stream).Read({ &(__storage[0]), 1 }), SRX_READ_ERROR); \
  EXPECT_EQ((__stream).Write({ &(__storage[0]), 1 }), SRX_WRITE_ERROR)


#define RONLY_STREAM_CHECK_VALID(__stream, __buffer, __size) \
  EXPECT_TRUE((__stream).IsOpen());                          \
  EXPECT_EQ((__stream).GetSize(), __size);                   \
  EXPECT_EQ((__stream).GetLength(), __size);                 \
  EXPECT_EQ((__stream).begin(), __buffer);                   \
  EXPECT_EQ((__stream).end(), __buffer + __size);            \
  EXPECT_EQ((__stream).GetData(), __buffer);                 \
  RONLY_STREAM_CHECK_ACCESS((__stream), true);               \
  EXPECT_EQ((__stream).GetName(), StringView());             \
  EXPECT_EQ((__stream).GetLength(), __size);                 \
  EXPECT_EQ((__stream).GetPosition(), 0);                    \
  EXPECT_EQ((__stream).GetStatus().GetCode(), (int)EStatusCode::Ok)

#define RONLY_STREAM_CHECK_WITH_POSITION(__stream,        \
                                         __buffer,        \
                                         __size,          \
                                         __position)      \
  EXPECT_TRUE(__stream.IsOpen());                         \
  EXPECT_EQ((__stream).GetSize(), __size);                \
  EXPECT_EQ(__stream.GetLength(), (__size - __position)); \
  EXPECT_EQ((__stream).begin(), __buffer);                \
  EXPECT_EQ(__stream.end(), __buffer + __size);           \
  EXPECT_EQ((__stream).GetData(), __buffer);              \
  RONLY_STREAM_CHECK_ACCESS(__stream, true);              \
  EXPECT_EQ((__stream).GetName(), StringView());          \
  EXPECT_EQ(__stream.GetLength(), __size - __position);   \
  EXPECT_EQ((__stream).GetPosition(), __position);        \
  EXPECT_EQ((__stream).GetStatus().GetCode(), (int)EStatusCode::Ok)

TEST(ReadOnlyMemoryStream, Construction)
{
  constexpr size_t kBufferSize = 256;
  byte             buffer[kBufferSize];
  byte             storage[kBufferSize];

  for (size_t i = 0; i < kBufferSize; ++i)
    buffer[i] = (byte)i;

  // Default Constructor
  ReadOnlyMemoryStream stream_empty;
  EXPECT_TRUE(stream_empty.IsA<ReadOnlyMemoryStream>());
  EXPECT_TRUE(stream_empty.IsA<Stream>());
  RONLY_STREAM_CHECK_INVALID(stream_empty, storage, EStatusCode::Ok);

  // Copy Constructor
  ReadOnlyMemoryStream st_copy_empty(stream_empty);
  RONLY_STREAM_CHECK_INVALID(st_copy_empty,
                             storage,
                             stream_empty.GetStatus().GetCode());
  // Constructor
  ReadOnlyMemoryStream stream_buffer(buffer, kBufferSize);
  RONLY_STREAM_CHECK_VALID(stream_buffer, buffer, kBufferSize);

  // Copy Constructor
  TUniquePointer<ReadOnlyMemoryStream> stream =
    MakeUnique<ReadOnlyMemoryStream>(stream_buffer);
  RONLY_STREAM_CHECK_VALID((*stream), buffer, kBufferSize);

  // Assign Operator
  *stream = stream_empty;
  RONLY_STREAM_CHECK_INVALID(st_copy_empty,
                             storage,
                             stream_empty.GetStatus().GetCode());

  // Check Assing Operator: Position
  EXPECT_TRUE(stream_buffer.Seek(kBufferSize / 2, ESeekMode::Begin));
  *stream = stream_buffer;
  RONLY_STREAM_CHECK_WITH_POSITION((*stream),
                                   buffer,
                                   kBufferSize,
                                   (kBufferSize / 2));
  *stream = std::move(st_copy_empty);
  RONLY_STREAM_CHECK_INVALID(st_copy_empty,
                             storage,
                             st_copy_empty.GetStatus().GetCode());
  // Data Copy
  stream = MakeUnique<ReadOnlyMemoryStream>(
    buffer,
    buffer + kBufferSize,
    ReadOnlyMemoryStream::EParameters::Data_Copy);

  EXPECT_TRUE(stream->IsOpen());
  EXPECT_EQ(stream->GetSize(), kBufferSize);
  EXPECT_EQ(stream->GetLength(), kBufferSize);
  EXPECT_FALSE(stream->begin() == nullptr || stream->begin() == buffer);
  EXPECT_EQ(stream->GetData(), stream->begin());
  RONLY_STREAM_CHECK_ACCESS((*stream), true);
  EXPECT_EQ(stream->GetLength(), kBufferSize);
  EXPECT_EQ(stream->GetPosition(), 0);
  EXPECT_EQ(stream->GetStatus().GetCode(), (int)EStatusCode::Ok);

  ASSERT_EQ(stream->Read({ &(storage[0]), kBufferSize }), ssize_t(kBufferSize));
  ASSERT_TRUE(stream->EndOfFile());

  auto cpstream = MakeUnique<ReadOnlyMemoryStream>(*stream);
  EXPECT_TRUE(cpstream->IsOpen());
  EXPECT_EQ(cpstream->GetSize(), kBufferSize);
  EXPECT_EQ(cpstream->GetLength(), 0);
  EXPECT_FALSE(cpstream->begin() == nullptr
               || cpstream->begin() == stream->begin());
  EXPECT_EQ(cpstream->GetData(), cpstream->begin());
  RONLY_STREAM_CHECK_ACCESS((*cpstream), true);
  EXPECT_EQ(cpstream->GetSize(), kBufferSize);

  EXPECT_TRUE(cpstream->Reset());

  // Move Operator
  *cpstream = std::move(*stream);
  EXPECT_TRUE(cpstream->IsOpen());
  EXPECT_EQ(cpstream->GetSize(), kBufferSize);
  EXPECT_EQ(cpstream->GetLength(), 0);
  EXPECT_FALSE(cpstream->begin() == nullptr || stream->begin() == buffer);
  EXPECT_EQ(cpstream->GetData(), cpstream->begin());
  RONLY_STREAM_CHECK_ACCESS((*cpstream), true);
  ASSERT_TRUE(stream->EndOfFile());

  // Move Constructor
  stream.reset();
  stream = MakeUnique<ReadOnlyMemoryStream>(std::move(*cpstream));
  EXPECT_TRUE(stream->IsOpen());
  EXPECT_EQ(stream->GetSize(), kBufferSize);
  EXPECT_FALSE(stream->begin() == nullptr || stream->begin() == buffer);
  EXPECT_EQ(stream->GetData(), stream->begin());
  RONLY_STREAM_CHECK_ACCESS((*stream), true);
  EXPECT_EQ(stream->GetLength(), 0);
  ASSERT_TRUE(stream->EndOfFile());
}

TEST(ReadOnlyMemoryStream, Common)
{
  constexpr size_t kBufferSize = 256;
  byte             buffer[kBufferSize];

  for (size_t i = 0; i < kBufferSize; ++i)
    buffer[i] = (byte)i;

  byte value;

  constexpr size_t kSmallStorateSize = kBufferSize / 2;
  constexpr size_t kBigStorageSize   = 2 * kBufferSize;
  byte             storage[kBigStorageSize];

  ReadOnlyMemoryStream stream(buffer, buffer + kBufferSize);
  RONLY_STREAM_CHECK_VALID(stream, buffer, kBufferSize);

  // Read & Peek
  for (size_t i = 0; i <= kBufferSize; ++i)
  {
    if (i == kBufferSize)
    {
      EXPECT_FALSE(stream.Peek(value));
      EXPECT_EQ(stream.GetPosition(), kBufferSize);
      EXPECT_TRUE(stream.EndOfFile());

      EXPECT_FALSE(stream.Next(value));
      EXPECT_NE(stream.GetStatus().GetCode(), (int)EStatusCode::Ok);
    }
    else
    {
      ASSERT_TRUE(stream.Peek(value));
      ASSERT_EQ(value, i);
      ASSERT_EQ(stream.GetPosition(), i);
      ASSERT_EQ(stream.GetLength(), kBufferSize - i);
      EXPECT_FALSE(stream.EndOfFile());

      ASSERT_TRUE(stream.Next(value));
      ASSERT_EQ(stream.GetPosition(), i + 1);
      ASSERT_EQ(stream.GetLength(), kBufferSize - i - 1);
    }
  }

  EXPECT_TRUE(stream.Reset());
  RONLY_STREAM_CHECK_VALID(stream, buffer, kBufferSize);
  ASSERT_FALSE(stream.Seek(-1, ESeekMode::Begin));
  EXPECT_TRUE(stream.Reset());
  RONLY_STREAM_CHECK_VALID(stream, buffer, kBufferSize);

  // Seek & Peek
  for (int32 i = 0; i <= kBufferSize; ++i)
  {
    if (i == kBufferSize)
    {
      ASSERT_FALSE(stream.Seek(i, ESeekMode::Begin));
      EXPECT_TRUE(stream.Peek(value));
      EXPECT_EQ(stream.GetPosition(), kBufferSize - 1);
      EXPECT_FALSE(stream.EndOfFile());
    }
    else
    {
      ASSERT_TRUE(stream.Seek(i, ESeekMode::Begin));
      ASSERT_EQ(stream.GetPosition(), i);
      ASSERT_EQ(stream.GetLength(), kBufferSize - i);

      ASSERT_TRUE(stream.Peek(value));
      ASSERT_EQ(value, i);
      ASSERT_EQ(stream.GetPosition(), i);
      ASSERT_EQ(stream.GetLength(), kBufferSize - i);
    }
  }

  EXPECT_TRUE(stream.Reset());
  RONLY_STREAM_CHECK_VALID(stream, buffer, kBufferSize);

  // Next & Get
  for (int32 i = 0; i < kBufferSize; ++i)
  {
    EXPECT_EQ(stream[i], i);
    ASSERT_TRUE(stream.Get(i, value));
    ASSERT_EQ(value, i);
    ASSERT_EQ(stream.NextUnsafe(), i);

    ASSERT_EQ(stream.GetPosition(), i + 1);
    ASSERT_EQ(stream.GetLength(), kBufferSize - i - 1);
  }

  EXPECT_TRUE(stream.EndOfFile());
  EXPECT_FALSE(stream.Get(kBufferSize, value));

  EXPECT_TRUE(stream.Reset());
  RONLY_STREAM_CHECK_VALID(stream, buffer, kBufferSize);

  // Advance
  int32 i = 0, pos = 0, last = 0;
  while (i < kBufferSize)
  {
    i = (i % 2) ? 2 * i : (i + 1);
    pos += i;
    if (pos >= kBufferSize)
    {
      EXPECT_FALSE(stream.Advance(i));
      EXPECT_EQ(stream.GetPosition(), last);
    }
    else
    {
      // advance
      stream.Advance(i);
      ASSERT_EQ(stream.GetPosition(), pos);
      ASSERT_EQ(stream.Get(pos, value), true);
      ASSERT_EQ(value, buffer[pos]);

      // step back
      ASSERT_TRUE(stream.Seek(-i, ESeekMode::Current));
      ASSERT_EQ(stream.GetPosition(), pos - i);

      // advance save
      ASSERT_TRUE(stream.Advance(i));
      ASSERT_EQ(stream.GetPosition(), pos);
      ASSERT_EQ(stream.Get(pos, value), true);
      ASSERT_EQ(value, buffer[pos]);

      last = pos;
    }
  }
}

TEST(ReadOnlyMemoryStream, ReadIntUnsafe)
{
  constexpr size_t kBufferSize         = 64;
  byte             buffer[kBufferSize] = { 0 };

  for (size_t i = 0; i < kBufferSize; ++i)
    buffer[i] = i;

  ReadOnlyMemoryStream stream(buffer, buffer + kBufferSize);
  RONLY_STREAM_CHECK_VALID(stream, buffer, kBufferSize);

  const uint16 u16 = 0x0001;
  const int16  i16 = 0x0203;
  const uint32 u32 = 0x04050607;
  const int32  i32 = 0x08090a0b;
  const uint64 u64 = 0x0c0d0e0f10111213;
  const uint8  u8  = 0x14;
  const int64  i64 = 0x15161718191a1b1c;

  uint16 tu16;
  int16  ti16;
  uint32 tu32;
  int32  ti32;
  uint64 tu64;
  uint8  tu8;
  int64  ti64;

  size_t n = 0;
  stream.ReadIntUnsafe(tu16);
  n += sizeof(tu16);
  ASSERT_EQ(stream.GetLength(), kBufferSize - n);
  EXPECT_EQ(tu16, u16);

  stream.ReadIntUnsafe(ti16);
  n += sizeof(ti16);
  ASSERT_EQ(stream.GetLength(), kBufferSize - n);
  EXPECT_EQ(ti16, i16);

  stream.ReadIntUnsafe(tu32);
  n += sizeof(tu32);
  ASSERT_EQ(stream.GetLength(), kBufferSize - n);
  EXPECT_EQ(tu32, u32);

  stream.ReadIntUnsafe(ti32);
  n += sizeof(ti32);
  ASSERT_EQ(stream.GetLength(), kBufferSize - n);
  EXPECT_EQ(ti32, i32);

  stream.ReadIntUnsafe(tu64);
  n += sizeof(tu64);
  ASSERT_EQ(stream.GetLength(), kBufferSize - n);
  EXPECT_EQ(tu64, u64);

  stream.ReadIntUnsafe(tu8);
  n += sizeof(tu8);
  ASSERT_EQ(stream.GetLength(), kBufferSize - n);
  EXPECT_EQ(tu8, u8);

  stream.ReadIntUnsafe(ti64);
  n += sizeof(ti64);
  ASSERT_EQ(stream.GetLength(), kBufferSize - n);
  EXPECT_EQ(ti64, i64);

  // Reverse bytes
  const uint16 ru16 = 0x0100;
  const int16  ri16 = 0x0302;
  const uint32 ru32 = 0x07060504;
  const int32  ri32 = 0x0b0a0908;
  const uint64 ru64 = 0x131211100f0e0d0c;
  const uint8  ru8  = 0x14;
  const int64  ri64 = 0x1c1b1a1918171615;

  const auto opt =
    Utils::IsLittleEndian()
      ? ReadOnlyMemoryStream::EParameters::Integer_BytesSwap_Disable
      : ReadOnlyMemoryStream::EParameters::Integer_BytesSwap_Enable;
  stream = ReadOnlyMemoryStream(buffer, kBufferSize, opt);
  RONLY_STREAM_CHECK_VALID(stream, buffer, kBufferSize);

  n = 0;
  stream.ReadIntUnsafe(tu16);
  n += sizeof(tu16);
  ASSERT_EQ(stream.GetLength(), kBufferSize - n);
  EXPECT_EQ(tu16, ru16);

  stream.ReadIntUnsafe(ti16);
  n += sizeof(ti16);
  ASSERT_EQ(stream.GetLength(), kBufferSize - n);
  EXPECT_EQ(ti16, ri16);

  stream.ReadIntUnsafe(tu32);
  n += sizeof(tu32);
  ASSERT_EQ(stream.GetLength(), kBufferSize - n);
  EXPECT_EQ(tu32, ru32);

  stream.ReadIntUnsafe(ti32);
  n += sizeof(ti32);
  ASSERT_EQ(stream.GetLength(), kBufferSize - n);
  EXPECT_EQ(ti32, ri32);

  stream.ReadIntUnsafe(tu64);
  n += sizeof(tu64);
  ASSERT_EQ(stream.GetLength(), kBufferSize - n);
  EXPECT_EQ(tu64, ru64);

  stream.ReadIntUnsafe(tu8);
  n += sizeof(tu8);
  ASSERT_EQ(stream.GetLength(), kBufferSize - n);
  EXPECT_EQ(tu8, ru8);

  stream.ReadIntUnsafe(ti64);
  n += sizeof(ti64);
  ASSERT_EQ(stream.GetLength(), kBufferSize - n);
  EXPECT_EQ(ti64, ri64);
}

TEST(ReadOnlyMemoryStream, Read)
{
  // TODO: Make Test Prep
  constexpr size_t kBufferSize = 0xFF;
  byte             buffer[kBufferSize];

  for (size_t i = 0; i < kBufferSize; ++i)
    buffer[i] = (byte)i;

  byte value;

  constexpr size_t kSmallStorateSize        = kBufferSize / 2;
  constexpr size_t kBigStorageSize          = 2 * kBufferSize;
  byte             storage[kBigStorageSize] = { 0 };

  ReadOnlyMemoryStream stream(buffer, kBufferSize);
  RONLY_STREAM_CHECK_VALID(stream, buffer, kBufferSize);

  EXPECT_EQ(stream.Read({ &(storage[0]), kBufferSize }), kBufferSize);
  EXPECT_EQ(stream.GetStatus().GetCode(), (int)EStatusCode::Ok);
  for (size_t i = 0; i < kBufferSize; ++i)
    ASSERT_EQ(buffer[i], storage[i]);

  EXPECT_TRUE(stream.EndOfFile());
  EXPECT_EQ(stream.Read({ &(storage[0]), 1 }), SRX_READ_ERROR);
  EXPECT_NE(stream.GetStatus().GetCode(), (int)EStatusCode::Ok);

  EXPECT_TRUE(stream.Reset());
  RONLY_STREAM_CHECK_VALID(stream, buffer, kBufferSize);

  std::fill(storage, storage + kBufferSize, 0);
  EXPECT_EQ(stream.Read({ &(storage[0]), kBigStorageSize }), kBufferSize);
  for (size_t i = 0; i < kBufferSize; ++i)
    ASSERT_EQ(buffer[i], storage[i]);

  EXPECT_TRUE(stream.Reset());
  RONLY_STREAM_CHECK_VALID(stream, buffer, kBufferSize);
  std::fill(storage, storage + kBufferSize, 0);

  for (size_t i = 1; i < kBufferSize; i += 2)
  {
    ssize_t pos;
    pos = stream.GetPosition();
    if (pos + i >= stream.GetSize())
      break;

    ASSERT_EQ(stream.Read({ &(storage[0]), i }), i);
    for (size_t j = 0; j < i; ++j)
      ASSERT_EQ(buffer[pos + j], storage[j]);
  }
}

TEST(ReadOnlyMemoryStream, ViewUnsafe)
{
  // TODO: Make Test Prep
  constexpr byte kBufferSize = 0xFF;
  byte           buffer[kBufferSize];

  for (size_t i = 0; i < kBufferSize; ++i)
    buffer[i] = (byte)i;

  ReadOnlyMemoryStream stream(buffer, kBufferSize);
  RONLY_STREAM_CHECK_VALID(stream, buffer, kBufferSize);

  auto dataView = stream.ViewUnsafe(10);
  EXPECT_EQ(stream.GetStatus(), SRX_OK);
  EXPECT_EQ(stream.GetLength(), kBufferSize - 10);

  for (byte i = 0; i < dataView.size(); i++)
    ASSERT_EQ(dataView[i], i);

  byte b = 10;
  for (; b < kBufferSize / 2; b++)
    ASSERT_EQ(stream.NextUnsafe(), b);

  dataView = stream.ViewUnsafe(stream.GetLength());
  EXPECT_EQ(stream.GetStatus(), SRX_OK);
  EXPECT_EQ(stream.GetLength(), 0);
  EXPECT_TRUE(stream.EndOfFile());

  for (size_t i = 0; b < dataView.size(); i++, b++)
    ASSERT_EQ(dataView[i], b);
}
