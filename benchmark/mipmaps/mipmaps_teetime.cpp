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

#include <iostream>
#include <teetime/stages/RandomIntProducer.h>
#include <teetime/stages/AbstractConsumerStage.h>
#include <teetime/stages/AbstractFilterStage.h>
#include <teetime/stages/FunctionStage.h>
#include <teetime/stages/CollectorSink.h>
#include <teetime/stages/DistributorStage.h>
#include <teetime/stages/MergerStage.h>
#include <teetime/Configuration.h>
#include <teetime/Image.h>
#include <teetime/logging.h>
#include <teetime/platform.h>
#include <teetime/BufferedFile.h>
#include <climits>
#include <string>
#include <algorithm>
#include <fstream>
#include "../Benchmark.h"

using namespace teetime;

std::string getImageInputDirectory(int num, int size);
std::string getImageOutputDirectory();

namespace {
  struct MipMapTask
  {
    MipMapTask() = default;
    MipMapTask(const MipMapTask&) = default;
    MipMapTask(MipMapTask&& task)
      : sourceImage(std::move(task.sourceImage))
      , filename(std::move(task.filename))
      , level(task.level)
    {}

    shared_ptr<const Image> sourceImage;
    std::string filename;
    size_t level;
  };

  class Producer : public AbstractProducerStage<MipMapTask>
  {
  public:
    Producer(int num, int size)
      : m_size(size)
      , m_num(num)
    {}

  private:
    virtual void execute() override
    {
      auto dir = getImageInputDirectory(m_num, m_size);
      std::vector<std::string> filenames;
      platform::listFiles(dir, filenames, true);

      for (const auto& f : filenames)
      {
        BufferedFile bf;
        if (!bf.load(dir + "/" +f))
          continue;

        auto image = std::make_shared<Image>();
        if (!image->loadFromMemory(bf.data(), bf.size(), bf.path()))
          continue;

        size_t level = 0;
        size_t width = image->getWidth();
        size_t height = image->getHeight();



        while (height > 0 || width > 0)
        {
          MipMapTask task;
          task.sourceImage = image;
          task.filename = f;
          task.level = level;

          getOutputPort().send(std::move(task));

          level += 1;
          width /= 2;
          height /= 2;
        }
      }

      AbstractProducerStage<MipMapTask>::terminate();
    }

    int m_size;
    int m_num;
  };

  class MipMap : public AbstractFilterStage<MipMapTask, std::string>
  {
  public:
    MipMap()
      : m_outputdir(getImageOutputDirectory())
    {
    }

  private:
    virtual void execute(MipMapTask&& task) override
    {
      if (!task.sourceImage)
        return;

      size_t width = task.sourceImage->getWidth() / (size_t(1) << task.level);
      size_t height = task.sourceImage->getHeight() / (size_t(1) << task.level);
      width = std::max<size_t>(width, 1);
      height = std::max<size_t>(height, 1);

      auto image = task.sourceImage->resize(width, height);

      char filename[256];
      sprintf(filename, "%s/%d_%s", m_outputdir.c_str(), static_cast<int>(task.level), task.filename.c_str());
      image.saveToPngFile(filename);

      getOutputPort().send(std::string(filename));
    }

    std::string m_outputdir;
  };

  class Config : public Configuration
  {
  public:
    Config(const Params& params, int threads, const std::vector<int>& affinity)
    {
      int num = params.getInt32("num");
      int size = params.getInt32("minvalue");

      CpuDispenser cpus(affinity);

      auto producer = createStage<Producer>(num, size);
      auto dist = createStage<DistributorStage<MipMapTask>>();
      auto merger = createStage<MergerStage<std::string>>();
      auto sink = createStage<CollectorSink<std::string>>();

      declareStageActive(producer, cpus.next());
      declareStageActive(merger, cpus.next());

      connectPorts(producer->getOutputPort(), dist->getInputPort());
      connectPorts(merger->getOutputPort(), sink->getInputPort());

      for (int i = 0; i < threads; ++i)
      {
        auto mipmap = createStage<MipMap>();
        declareStageActive(mipmap);
        connectPorts(dist->getNewOutputPort(), mipmap->getInputPort());
        connectPorts(mipmap->getOutputPort(), merger->getNewInputPort());
      }
    }
  };

}



void mipmaps_teetime_noAffinity(const Params& params, int threads)
{
  Config config(params, threads, affinity_none);
  config.executeBlocking();
}

void mipmaps_teetime_preferSameCpu(const Params& params, int threads)
{
  Config config(params, threads, affinity_preferSameCpu);
  config.executeBlocking();
}

void mipmaps_teetime_avoidSameCore(const Params& params, int threads)
{
  Config config(params, threads, affinity_avoidSameCore);
  config.executeBlocking();
}