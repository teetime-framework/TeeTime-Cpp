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
#include <teetime/stages/ReadImage.h>
#include <teetime/File.h>
#include <teetime/FileBuffer.h>
#include <teetime/Image.h>

using namespace teetime;

namespace
{
  inline std::string getFilePath(const std::string& name)
  {
    return std::string(TEETIME_LOCAL_TEST_DIR "/stages/ReadImageTest/") + name;
  }

  class ReadImageTestConfig : public Configuration
  {
  public:
    shared_ptr<CollectorSink<Image>> images;

    explicit ReadImageTestConfig(const File& files)
    {
      auto producer = createStage<InitialElementProducer<File>>(files);
      auto file2buffer = createStage<File2FileBuffer>();
      auto readImage = createStage<ReadImage>();
      images = createStage<CollectorSink<Image>>();

      declareStageActive(producer);
      connectPorts(producer->getOutputPort(), file2buffer->getInputPort());
      connectPorts(file2buffer->getOutputPort(), readImage->getInputPort());
      connectPorts(readImage->getOutputPort(), images->getInputPort());      
    }
  };
}

TEST(ReadImageTest, png)
{
  ReadImageTestConfig config(File(getFilePath("lena.png")));
  config.executeBlocking();

  auto images = config.images->takeElements();

  ASSERT_EQ((size_t)1, images.size());
  EXPECT_EQ((size_t)512, images[0].getHeight());
  EXPECT_EQ((size_t)512, images[0].getWidth());
  EXPECT_EQ(getFilePath("lena.png"), images[0].getFilename());
}

TEST(ReadImageTest, jpg)
{
  ReadImageTestConfig config(File(getFilePath("lena.jpg")));
  config.executeBlocking();

  auto images = config.images->takeElements();

  ASSERT_EQ((size_t)1, images.size());
  EXPECT_EQ((size_t)512, images[0].getHeight());
  EXPECT_EQ((size_t)512, images[0].getWidth());
  EXPECT_EQ(getFilePath("lena.jpg"), images[0].getFilename());
}

TEST(ReadImageTest, tga)
{
  ReadImageTestConfig config(File(getFilePath("lena.tga")));
  config.executeBlocking();

  auto images = config.images->takeElements();

  ASSERT_EQ((size_t)1, images.size());
  EXPECT_EQ((size_t)512, images[0].getHeight());
  EXPECT_EQ((size_t)512, images[0].getWidth());
  EXPECT_EQ(getFilePath("lena.tga"), images[0].getFilename());
}

