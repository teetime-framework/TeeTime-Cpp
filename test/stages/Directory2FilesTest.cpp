/**
* Copyright (C) 2016 Johannes Ohlemacher (https://github.com/teetime-framework/TeeTime-Cpp)
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
#include <teetime/stages/Directory2Files.h>
#include <teetime/File.h>

using namespace teetime;

namespace
{
  class Directory2FilesTestConfig : public Configuration
  {
  public:
    shared_ptr<CollectorSink<File>> collector;

    explicit Directory2FilesTestConfig()
    {
      auto producer = createStage<InitialElementProducer<std::string>>(std::string(TEETIME_LOCAL_TEST_DIR "/stages/Directory2FilesTest"));
      auto dir2files = createStage<Directory2Files>();
      collector = createStage<CollectorSink<File>>();

      declareStageActive(producer);
      connectPorts(producer->getOutputPort(), dir2files->getInputPort());
      connectPorts(dir2files->getOutputPort(), collector->getInputPort());
    }
  };

  inline std::string getFilePath(const char* name)
  {
    return std::string(TEETIME_LOCAL_TEST_DIR "/stages/Directory2FilesTest/") + name;
  }
}

TEST(Directory2FilesTest, simple)
{
  Directory2FilesTestConfig config;

  config.executeBlocking();

  std::vector<File> files = config.collector->takeElements();

  ASSERT_EQ((size_t)6 ,files.size());
  EXPECT_EQ(getFilePath("dir1/dir2/file5.txt"), files[0].path);
  EXPECT_EQ(getFilePath("dir1/file3.txt"), files[1].path);
  EXPECT_EQ(getFilePath("dir1/file4.txt"), files[2].path);
  EXPECT_EQ(getFilePath("dir3/file6.txt"), files[3].path);
  EXPECT_EQ(getFilePath("file1.txt"), files[4].path);
  EXPECT_EQ(getFilePath("file2.txt"), files[5].path);
}

