/**
 * Copyright (C) 2016 Johannes Ohlemacher (https://github.com/eXistence/TeeTime-Cpp)
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *         http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */
#include <gtest/gtest.h>
#include <teetime/pipes/SpscQueue.h>
#include <thread>
#include <atomic>

using namespace teetime;


class SpscValueQueueTest : public ::testing::TestWithParam<int> {

};

TEST_P(SpscValueQueueTest, concurrent)
{
  static const size_t numValues = 10000000;
  SpscValueQueue<int> queue(GetParam());
  std::vector<int> dst;
  dst.reserve(numValues);
  bool producerDone = false;
  
  std::thread producer([&]() {
    for (size_t i = 0; i < numValues; ++i)
    {
      while(!queue.write(int(i)))
      {
        std::this_thread::yield();
      }
    }
    producerDone = true;
  });

  std::thread consumer([&]() {
    while (true)
    {
      if(auto front = queue.frontPtr())
      {
        dst.push_back(*front);
        queue.popFront();
      }
      else if (producerDone) //re-check pipe.size, because the other thread could have added elements in the meantime
      {
        break;
      }
      else
      {
        std::this_thread::yield();
      }
    }
  });

  producer.join();
  consumer.join();

  ASSERT_EQ(numValues, dst.size());
  for (size_t i = 0; i < dst.size(); ++i)
  {
    EXPECT_EQ((int)i, dst[i]);
  }
}

INSTANTIATE_TEST_CASE_P(Foobar, SpscValueQueueTest, ::testing::Values(2, 16, 1024, 4096));

















class SpscValueQueueTest2 : public ::testing::TestWithParam<int> {

};

TEST_P(SpscValueQueueTest2, concurrent)
{
  using intlist = std::vector<int>;

  static const size_t numValues = 10000000;
  SpscValueQueue<intlist> queue(GetParam());
  
  std::vector<intlist> dst;
  std::vector<intlist> src;
  dst.reserve(numValues);
  src.reserve(numValues);

  for (size_t i = 0; i < numValues; ++i)
  {
    src.push_back(intlist(64, (int)i));
  }

  
  std::thread producer([&]() {
    for (size_t i = 0; i < numValues; ++i)
    {
      while(!queue.write(std::move(src[i])))
      {
        std::this_thread::yield();
      }
    }
  });

  std::thread consumer([&]() {
    while (dst.size() < src.size())
    {
      if(auto front = queue.frontPtr())
      {
        dst.push_back(std::move(*front));
        queue.popFront();
      }
      else
      {
        std::this_thread::yield();
      }
    }
  });

  producer.join();
  consumer.join();

  ASSERT_EQ(numValues, dst.size());
  for (size_t i = 0; i < dst.size(); ++i)
  {
    ASSERT_EQ((size_t)64, dst[i].size());
    for(auto v : dst[i])
    {
      EXPECT_EQ((int)i, v);
    }
  }
}

INSTANTIATE_TEST_CASE_P(Foobar, SpscValueQueueTest2, ::testing::Values(2, 16, 1024, 4096));














class SpscValueQueueTest3 : public ::testing::TestWithParam<int> {

};

TEST_P(SpscValueQueueTest3, concurrent)
{
  static const size_t numValues = 10000000;
  SpscValueQueue<shared_ptr<int>> queue(GetParam());

  std::vector<shared_ptr<int>> dst;
  std::vector<shared_ptr<int>> src;
  dst.reserve(numValues);
  src.reserve(numValues);

  for (size_t i = 0; i < numValues; ++i)
  {
    src.push_back(std::make_shared<int>((int)i));
  }


  std::thread producer([&]() {
    for (size_t i = 0; i < numValues; ++i)
    {
      while (!queue.write(std::move(src[i])))
      {
        std::this_thread::yield();
      }
    }
  });

  std::thread consumer([&]() {
    while (dst.size() < src.size())
    {
      if (auto front = queue.frontPtr())
      {
        dst.push_back(std::move(*front));
        queue.popFront();
      }
      else
      {
        std::this_thread::yield();
      }
    }
  });

  producer.join();
  consumer.join();

  ASSERT_EQ(numValues, dst.size());
  for (size_t i = 0; i < dst.size(); ++i)
  {
    ASSERT_TRUE(dst[i].get() != nullptr);
    ASSERT_EQ((int)i, *dst[i]);
  }
}

INSTANTIATE_TEST_CASE_P(Foobar, SpscValueQueueTest3, ::testing::Values(2, 16, 1024, 4096));














class SpscValueQueueTest4 : public ::testing::TestWithParam<int> {

};

TEST_P(SpscValueQueueTest4, concurrent)
{
  static const size_t numValues = 10000000;
  SpscValueQueue<unique_ptr<int>> queue(GetParam());

  std::vector<unique_ptr<int>> dst;
  std::vector<unique_ptr<int>> src;
  dst.reserve(numValues);
  src.reserve(numValues);

  for (size_t i = 0; i < numValues; ++i)
  {
    src.push_back(std::unique_ptr<int>(new int((int)i)));
  }


  std::thread producer([&]() {
    for (size_t i = 0; i < numValues; ++i)
    {
      while (!queue.write(std::move(src[i])))
      {
        std::this_thread::yield();
      }
    }
  });

  std::thread consumer([&]() {
    while (dst.size() < src.size())
    {
      if (auto front = queue.frontPtr())
      {
        dst.push_back(std::move(*front));
        queue.popFront();
      }
      else
      {
        std::this_thread::yield();
      }
    }
  });

  producer.join();
  consumer.join();

  ASSERT_EQ(numValues, src.size());
  for (size_t i = 0; i < src.size(); ++i)
  {
    ASSERT_TRUE(src[i].get() == nullptr);
  }

  ASSERT_EQ(numValues, dst.size());
  for (size_t i = 0; i < dst.size(); ++i)
  {
    ASSERT_TRUE(dst[i].get() != nullptr);
    ASSERT_EQ((int)i, *dst[i]);
  }
}

INSTANTIATE_TEST_CASE_P(Foobar, SpscValueQueueTest4, ::testing::Values(2, 16, 1024, 4096));












class SpscPointerQueueTest : public ::testing::TestWithParam<int> {

};

TEST_P(SpscPointerQueueTest, concurrent)
{
  static const size_t numValues = 10000000;
  SpscPointerQueue<int*> queue(GetParam());
  std::vector<int*> dst;
  dst.reserve(numValues);
  bool producerDone = false;
  
  std::thread producer([&]() {
    for (size_t i = 0; i < numValues; ++i)
    {
      int* p = (int*)(i+1);
      while(!queue.write(p))
      {
        std::this_thread::yield();
      }
    }
    producerDone = true;
  });

  std::thread consumer([&]() {
    while (true)
    {
      int* value;
      if(queue.read(value))
      {
        dst.push_back(value);
      }
      else if (producerDone) //re-check pipe.size, because the other thread could have added elements in the meantime
      {
        break;
      }
      else
      {
        std::this_thread::yield();
      }
    }
  });

  producer.join();
  consumer.join();

  ASSERT_EQ(numValues, dst.size());
  for (size_t i = 0; i < dst.size(); ++i)
  {
    int* p = (int*)(i+1);
    EXPECT_EQ(p, dst[i]);
  }
}

INSTANTIATE_TEST_CASE_P(Foobar, SpscPointerQueueTest, ::testing::Values(2, 16, 1024, 4096));
