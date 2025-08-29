#include <gtest/gtest.h>

#include <Sorex/Containers/SxListenerContainer.h>

using namespace Sorex;

class TestListener
{
  public:
  explicit TestListener(int value)
    : _value(value)
  {}

  inline int GetNumber() const { return _value; }

  private:
  int _value = 0;
};

TEST(ListenerContainer, Add)
{
  TListenerContainer<TestListener> li;
  ASSERT_EQ(li.GetSize(), 0u);
  ASSERT_TRUE(li.IsEmpty());

  TestListener lst0(0);
  EXPECT_FALSE(li.Contains(lst0));

  EXPECT_TRUE(li.Add(lst0));
  ASSERT_EQ(li.GetSize(), 1u);
  ASSERT_FALSE(li.IsEmpty());
  EXPECT_TRUE(li.Contains(lst0));

  EXPECT_FALSE(li.Add(lst0));

  TestListener lst1(1);
  EXPECT_TRUE(li.Add(lst1));
  ASSERT_EQ(li.GetSize(), 2u);
  ASSERT_FALSE(li.IsEmpty());
  EXPECT_TRUE(li.Contains(lst1));

  EXPECT_FALSE(li.Add(lst1));
}

TEST(ListenerContainer, Main)
{
  TListenerContainer<TestListener> li;
  ASSERT_EQ(li.GetSize(), 0u);
  ASSERT_TRUE(li.IsEmpty());

  constexpr int kNumber = 32;
  TestListener* listeners[kNumber];
  for (int i = 0; i < kNumber; ++i)
  {
    TestListener* lst = new TestListener(i);
    listeners[i]      = lst;

    EXPECT_FALSE(li.Contains(*lst));

    li.Add(*lst);
    EXPECT_EQ(li.GetSize(), i + 1);
    EXPECT_FALSE(li.IsEmpty());
    EXPECT_TRUE(li.Contains(*lst));
  }

  // Remove during iteraton
  size_t size = li.GetSize();
  for (int i = 0; i < kNumber; ++i)
  {
    if (i % 2 != 0)
    {
      --size;
      li.Remove(*listeners[i]);
      EXPECT_EQ(li.GetSize(), size);
      EXPECT_FALSE(li.IsEmpty());
      EXPECT_FALSE(li.Contains(*listeners[i]));
    }
  }

  EXPECT_EQ(li.GetSize(), kNumber / 2);
  EXPECT_FALSE(li.IsEmpty());

  // Check that others are into container
  size = li.GetSize();
  for (int i = 0; i < kNumber; ++i)
  {
    if (i % 2 == 0)
    {
      EXPECT_TRUE(li.Contains(*listeners[i]));
    }

    // Remove by limit
    constexpr int kLimit = kNumber / 2;
    if (i > kLimit)
    {
      size = li.Contains(*listeners[i]) ? size - 1 : size;

      li.Remove(*listeners[i]);
      EXPECT_EQ(li.GetSize(), size);
      EXPECT_FALSE(li.IsEmpty());
      EXPECT_FALSE(li.Contains(*listeners[i]));
    }
  }

  EXPECT_FALSE(li.IsEmpty());
  li.Clear();
  EXPECT_TRUE(li.IsEmpty());

  // Free
  for (int i = 0; i < kNumber; ++i)
  {
    delete listeners[i];
    listeners[i] = nullptr;
  }
}

TEST(ListenerContainer, ReverseIterator)
{
  TListenerContainer<TestListener> li;
  ASSERT_EQ(li.GetSize(), 0u);
  ASSERT_TRUE(li.IsEmpty());

  constexpr int kNumber = 32;
  TestListener* listeners[kNumber];
  for (int i = 0; i < kNumber; ++i)
  {
    TestListener* lst = new TestListener(i);
    listeners[i]      = lst;
    li.Add(*lst);
  }

  // Iterate in reverse and check values
  EXPECT_EQ(li.GetSize(), kNumber);
  int expected = kNumber - 1;
  for (auto it = li.rbegin(); it != li.rend(); ++it)
  {
    EXPECT_EQ(it->GetNumber(), expected);
    --expected;
  }
  EXPECT_EQ(expected, -1);

  // Remove every 3rd listener during reverse iteration
  size_t size = li.GetSize();
  int    idx  = kNumber - 1;
  for (auto it = li.rbegin(); it != li.rend();)
  {
    if (idx % 3 == 0)
    {
      li.Remove(*it);
      --size;
      ++it;

      // Check that iterator is valid
      if (it != li.rend())
        EXPECT_EQ(it->GetNumber(), idx - 1);
    }
    else
    {
      ++it;
    }
    --idx;
  }
  EXPECT_EQ(li.GetSize(), kNumber - kNumber / 3 - (kNumber % 3 == 0 ? 0 : 1));

  // Check that removed listeners are not contained
  for (int i = 0; i < kNumber; ++i)
  {
    if (i % 3 == 0)
      EXPECT_FALSE(li.Contains(*listeners[i]));
    else
      EXPECT_TRUE(li.Contains(*listeners[i]));
  }

  // Clear and check
  li.Clear();
  EXPECT_TRUE(li.IsEmpty());

  // Free
  for (int i = 0; i < kNumber; ++i)
  {
    delete listeners[i];
    listeners[i] = nullptr;
  }
}
