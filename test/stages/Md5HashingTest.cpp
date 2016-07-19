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
#include <teetime/Configuration.h>
#include <teetime/stages/InitialElementProducer.h>
#include <teetime/stages/CollectorSink.h>
#include <teetime/stages/Md5Hashing.h>
#include <teetime/stages/benchmark/ReverseIntMd5Hashing.h>
#include <teetime/Md5Hash.h>
#include <teetime/ports/Port.h>

using namespace teetime;


TEST(Md5HashTest, roundtrip)
{
  auto bin = Md5Hash::generate("foobar");
  auto s = bin.toHexString();
  auto bin2 = Md5Hash::parseHexString(s);

  EXPECT_EQ(bin, bin2);
}

namespace
{
  class Md5HashTestConfig : public Configuration
  {
  public:
    shared_ptr<CollectorSink<Md5Hash>> hashes;

    explicit Md5HashTestConfig(const std::string& s)
    {
      auto producer = createStage<InitialElementProducer<std::string>>(s);
      auto md5 = createStage<Md5Hashing>();
      hashes = createStage<CollectorSink<Md5Hash>>();

      producer->declareActive();
      connect(producer->getOutputPort(), md5->getInputPort());
      connect(md5->getOutputPort(), hashes->getInputPort());
    }
  };
}

TEST(Md5HashTest, empty)
{
  Md5HashTestConfig config("");

  config.executeBlocking();

  auto hashes = config.hashes->takeElements();
  ASSERT_EQ((size_t)1, hashes.size());

  EXPECT_EQ(std::string("d41d8cd98f00b204e9800998ecf8427e"), hashes[0].toHexString());
}

TEST(Md5HashTest, example1)
{
  //source: https://en.wikipedia.org/wiki/MD5
  Md5HashTestConfig config("The quick brown fox jumps over the lazy dog");

  config.executeBlocking();

  auto hashes = config.hashes->takeElements();
  ASSERT_EQ((size_t)1, hashes.size());

  EXPECT_EQ(std::string("9e107d9d372bb6826bd81d3542a419d6"), hashes[0].toHexString());
}

TEST(Md5HashTest, example2)
{
  //source: https://en.wikipedia.org/wiki/MD5
  Md5HashTestConfig config("The quick brown fox jumps over the lazy dog."); //<= note the trailing dot

  config.executeBlocking();

  auto hashes = config.hashes->takeElements();
  ASSERT_EQ((size_t)1, hashes.size());

  EXPECT_EQ(std::string("e4d909c290d0fb1ca068ffaddf22cbd0"), hashes[0].toHexString());
}

namespace
{
  class ReverseIntMd5HashingTestConfig : public Configuration
  {
  public:
    shared_ptr<CollectorSink<int>> ints;

    explicit ReverseIntMd5HashingTestConfig(const std::vector<int> is)
    {
      std::vector<Md5Hash> hashes;
      hashes.reserve(is.size());
      for(auto i : is)
      {
        hashes.push_back(Md5Hash::generate(&i, sizeof(i)));
      }

      auto producer = createStage<InitialElementProducer<Md5Hash>>(hashes);
      auto md5 = createStage<ReverseIntMd5Hashing>();
      ints = createStage<CollectorSink<int>>();

      producer->declareActive();
      connect(producer->getOutputPort(), md5->getInputPort());
      connect(md5->getOutputPort(), ints->getInputPort());
    }
  };
}


TEST(Md5HashTest, reverse)
{
  std::vector<int> input = { 42, 2014, 123, 0, 1234567 };
  ReverseIntMd5HashingTestConfig config(input);

  config.executeBlocking();

  auto output = config.ints->takeElements();
  ASSERT_EQ(input, output);
}