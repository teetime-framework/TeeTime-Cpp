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
#include <teetime/stages/File2FileBuffer.h>
#include <teetime/ports/Port.h>
#include <teetime/File.h>
#include <teetime/FileBuffer.h>

using namespace teetime;

namespace
{
  inline std::string getFilePath(const std::string& name)
  {
    return std::string(TEETIME_LOCAL_TEST_DIR "/stages/File2FileBufferTest/") + name;
  }

  class File2FileBufferTestConfig : public Configuration
  {
  public:
    shared_ptr<CollectorSink<FileBuffer>> collector;

    explicit File2FileBufferTestConfig(const std::string& name)
    {
      auto producer = createStage<InitialElementProducer<File>>(File(getFilePath(name)));
      auto file2filebuffer = createStage<File2FileBuffer>();
      collector = createStage<CollectorSink<FileBuffer>>();

      producer->declareActive();
      connect(producer->getOutputPort(), file2filebuffer->getInputPort());
      connect(file2filebuffer->getOutputPort(), collector->getInputPort());
    }
  };
}

TEST(File2FileBuffer, simple)
{
  File2FileBufferTestConfig config("file1.txt");

  config.executeBlocking();

  std::vector<FileBuffer> files = config.collector->takeElements();

  ASSERT_EQ((size_t)1, files.size());
  EXPECT_EQ(getFilePath("file1.txt"), files[0].path);
  EXPECT_EQ((size_t)11, files[0].bytes.size());
  EXPECT_EQ(0, strncmp("hello world", (const char*)files[0].bytes.data(), files[0].bytes.size()));
}

TEST(File2FileBuffer, empty)
{
  File2FileBufferTestConfig config("empty.txt");

  config.executeBlocking();

  std::vector<FileBuffer> files = config.collector->takeElements();

  ASSERT_EQ((size_t)1, files.size());
  EXPECT_EQ(getFilePath("empty.txt"), files[0].path);
  EXPECT_EQ((size_t)0, files[0].bytes.size());
}

