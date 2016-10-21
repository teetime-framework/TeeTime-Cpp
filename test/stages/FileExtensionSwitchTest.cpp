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
#include <teetime/stages/FileExtensionSwitch.h>
#include <teetime/File.h>

using namespace teetime;

namespace
{
  class FileExtensionSwitchTestConfig : public Configuration
  {
  public:
    shared_ptr<CollectorSink<File>> txt;
    shared_ptr<CollectorSink<File>> tga;
    shared_ptr<CollectorSink<File>> noext;
    shared_ptr<CollectorSink<File>> other;

    explicit FileExtensionSwitchTestConfig(const std::vector<File>& files, bool useDefault)
    {
      auto producer = createStage<InitialElementProducer<File>>(files);
      auto fileExtSwitch = createStage<FileExtensionSwitch>();
      txt = createStage<CollectorSink<File>>();
      tga = createStage<CollectorSink<File>>();
      noext = createStage<CollectorSink<File>>();
      other = createStage<CollectorSink<File>>();

      declareStageActive(producer);
      connectPorts(producer->getOutputPort(), fileExtSwitch->getInputPort());
      connectPorts(fileExtSwitch->getOutputPort("txt"), txt->getInputPort());
      connectPorts(fileExtSwitch->getOutputPort("tga"), tga->getInputPort());
      connectPorts(fileExtSwitch->getOutputPort(""), noext->getInputPort());

      if (useDefault)
      {
        connectPorts(fileExtSwitch->getDefaultOutputPort(), other->getInputPort());
      }
    }
  };
}

TEST(FileExtensionSwitchTest, simple)
{
  FileExtensionSwitchTestConfig config({ 
    File("file1.txt"),
    File("file2.tga"),
    File("file3.txt"),
    File("file4.foo"),
    File("file5"),
    File("file6.tga"),
  }, true);

  config.executeBlocking();

  auto txtFiles = config.txt->takeElements();
  auto tgaFiles = config.tga->takeElements();
  auto noextFiles = config.noext->takeElements();
  auto otherFiles = config.other->takeElements();

  ASSERT_EQ((size_t)2, txtFiles.size());
  EXPECT_EQ(std::string("file1.txt"), txtFiles[0].path);
  EXPECT_EQ(std::string("file3.txt"), txtFiles[1].path);

  ASSERT_EQ((size_t)2, tgaFiles.size());
  EXPECT_EQ(std::string("file2.tga"), tgaFiles[0].path);
  EXPECT_EQ(std::string("file6.tga"), tgaFiles[1].path);

  ASSERT_EQ((size_t)1, noextFiles.size());
  EXPECT_EQ(std::string("file5"), noextFiles[0].path);

  ASSERT_EQ((size_t)1, otherFiles.size());
  EXPECT_EQ(std::string("file4.foo"), otherFiles[0].path);
}

TEST(FileExtensionSwitchTest, noDefault)
{
  FileExtensionSwitchTestConfig config({
    File("file1.txt"),
    File("file2.tga"),
    File("file3.txt"),
    File("file4.foo"),
    File("file5"),
    File("file6.tga"),
  }, false);

  config.executeBlocking();

  auto txtFiles = config.txt->takeElements();
  auto tgaFiles = config.tga->takeElements();
  auto noextFiles = config.noext->takeElements();
  auto otherFiles = config.other->takeElements();

  ASSERT_EQ((size_t)2, txtFiles.size());
  EXPECT_EQ(std::string("file1.txt"), txtFiles[0].path);
  EXPECT_EQ(std::string("file3.txt"), txtFiles[1].path);

  ASSERT_EQ((size_t)2, tgaFiles.size());
  EXPECT_EQ(std::string("file2.tga"), tgaFiles[0].path);
  EXPECT_EQ(std::string("file6.tga"), tgaFiles[1].path);

  ASSERT_EQ((size_t)1, noextFiles.size());
  EXPECT_EQ(std::string("file5"), noextFiles[0].path);

  ASSERT_EQ((size_t)0, otherFiles.size());
}
