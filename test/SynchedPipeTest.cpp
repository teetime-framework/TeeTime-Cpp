#include <gtest/gtest.h>
#include <teetime/SynchedPipe.h>
#include <thread>
#include <chrono>

using namespace teetime;

TEST(SynchedPipeTest, simple)
{
  SynchedPipe<int> pipe(1024);

  pipe.add(1);
  pipe.add(2);
  pipe.add(3);
  pipe.add(4);

  EXPECT_EQ(1, pipe.removeLast());
  EXPECT_EQ(2, pipe.removeLast());
  EXPECT_EQ(3, pipe.removeLast());
  EXPECT_EQ(4, pipe.removeLast());
}


TEST(SynchedPipeTest, concurrent)
{
  SynchedPipe<int> pipe(1024);
  std::vector<int> dst;
  bool producerDone = false;

  std::thread producer([&]() {
    for (int i = 0; i < 1000; ++i)
    {
      pipe.add(i);

      using namespace std::chrono_literals;
      if(i % 35 == 0)
        std::this_thread::sleep_for(100ms);
    }
    producerDone = true;
  });

  std::thread consumer([&]() {
    while (true)
    {
      if (pipe.size() > 0)
      {
        dst.push_back(pipe.removeLast());
      }
      else if (producerDone)
      {
        break;
      }
    }
  });

  producer.join();
  consumer.join();

  ASSERT_EQ(1000, dst.size());
  for (int i = 0; i < dst.size(); ++i)
  {
    EXPECT_EQ(i, dst[i]);
  }
}