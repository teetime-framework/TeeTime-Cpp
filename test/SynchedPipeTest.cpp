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
#include <teetime/pipes/SynchedPipe.h>
#include <thread>
#include <chrono>
#include <atomic>

using namespace teetime;

TEST(SynchedPipeTest, simple)
{
  SynchedPipe<int> pipe(1024);

  pipe.add(1);
  pipe.add(2);
  pipe.add(3);
  pipe.add(4);

  EXPECT_EQ(1, *pipe.removeLast());
  EXPECT_EQ(2, *pipe.removeLast());
  EXPECT_EQ(3, *pipe.removeLast());
  EXPECT_EQ(4, *pipe.removeLast());
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

      using std::chrono::milliseconds;
      
      if(i % 35 == 0)
        std::this_thread::sleep_for(milliseconds(50));
    }
    producerDone = true;
    TEETIME_INFO() << "producer is done, pipe size: " << pipe.size();
  });

  std::thread consumer([&]() {
    while (true)
    {
      if (pipe.size() > 0)
      {
        dst.push_back(*pipe.removeLast());
      }
      else if (producerDone && pipe.size() == 0) //re-check pipe.size, because the other thread could have added elements in the meantime
      {
        if(dst.size() < 1000)
        {
          TEETIME_ERROR() << "WTF? producer is done, but pipe is empty?";
        }
        else
        {
          break;
        }        
      }
    }
  });

  producer.join();
  consumer.join();

  ASSERT_EQ(size_t(1000), dst.size());
  for (size_t i = 0; i < dst.size(); ++i)
  {
    EXPECT_EQ((int)i, dst[i]);
  }
}