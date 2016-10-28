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

#include <iostream>
#include <cstdio>
#include <string>
#include <memory>
#include <thread>
#include <random> 

#include <teetime/Image.h>
#include <teetime/BufferedFile.h>
#include <teetime/logging.h>
#include <teetime/platform.h>
#include "../Benchmark.h"

TEETIME_WARNING_PUSH
TEETIME_WARNING_DISABLE_UNREFERENCED_PARAMETER
TEETIME_WARNING_DISABLE_EMPTY_BODY
TEETIME_WARNING_DISABLE_MISSING_FIELD_INIT
TEETIME_WARNING_DISABLE_PADDING_ALIGNMENT
TEETIME_WARNING_DISABLE_MAY_NOT_ALIGNED
TEETIME_WARNING_DISABLE_SIGNED_UNSIGNED_MISMATCH
TEETIME_WARNING_DISABLE_HIDDEN
TEETIME_WARNING_DISABLE_LOSSY_CONVERSION
TEETIME_WARNING_DISABLE_UNSAFE_USE_OF_BOOL
TEETIME_WARNING_DISABLE_UNREACHABLE

#define NO_VOLATILE_POINTERS
#include <ff/pipeline.hpp>
#include <ff/farm.hpp>

TEETIME_WARNING_POP

using namespace ff;
using teetime::Image;
using teetime::BufferedFile;
using teetime::Params;


std::string getImageInputDirectory(int num, int size);
std::string getImageOutputDirectory();

namespace
{
  struct MipMapTask
  {
    MipMapTask() = default;
    MipMapTask(const MipMapTask&) = default;
    MipMapTask(MipMapTask&& task)
      : sourceImage(std::move(task.sourceImage))
      , filename(std::move(task.filename))
      , level(task.level)
    {}

    std::shared_ptr<const Image> sourceImage;
    std::string filename;
    size_t level;
  };


  struct Producer : ff_node_t<char, MipMapTask>
  {
    Producer(int num, int size)
      : m_num(num)
      , m_size(size)
    {
    }

    MipMapTask* svc(char*)
    {
      using namespace teetime;

      auto dir = getImageInputDirectory(m_num, m_size);
      std::vector<std::string> filenames;
      platform::listFiles(dir, filenames, true);

      for (const auto& f : filenames)
      {
        BufferedFile bf;
        if (!bf.load(dir + "/" + f))
          continue;

        auto image = std::make_shared<Image>();
        if (!image->loadFromMemory(bf.data(), bf.size(), bf.path()))
          continue;

        size_t level = 0;
        size_t width = image->getWidth();
        size_t height = image->getHeight();

        while (height > 0 || width > 0)
        {
          MipMapTask* task = new MipMapTask();
          task->sourceImage = image;
          task->filename = f;
          task->level = level;

          ff_send_out(task);

          level += 1;
          width /= 2;
          height /= 2;
        }
      }

      return EOS;
    }

  private:
    int m_num;
    int m_size;
  };

  struct MipMap : ff_node_t<MipMapTask, std::string>
  {
    explicit MipMap()
      : m_outputdir(getImageOutputDirectory())
    {
    }

    std::string* svc(MipMapTask* task)
    {
      size_t width = task->sourceImage->getWidth() / (1 << task->level);
      size_t height = task->sourceImage->getHeight() / (1 << task->level);
      width = std::max<size_t>(width, 1);
      height = std::max<size_t>(height, 1);

      auto image = task->sourceImage->resize(width, height);

      char filename[256];
      sprintf(filename, "%s/%d_%s", m_outputdir.c_str(), static_cast<int>(task->level), task->filename.c_str());
      image.saveToPngFile(filename);

      delete task;
      return new std::string(filename);
    }

  private:
    std::string m_outputdir;
  };

  struct Sink : ff_node_t<std::string, char>
  {
    virtual char* svc(std::string* value) override
    {
      m_values.push_back(std::move(*value));
      delete value;
      return GO_ON;
    }

    std::vector<std::string> m_values;
  };

}

void mipmaps_fastflow(const Params& params, int threads)
{
  std::vector<std::unique_ptr<ff_node>> W;
  for (size_t i = 0; i < threads; ++i)
  {
    W.push_back(std::unique_ptr<ff_node_t<MipMapTask, std::string> >(make_unique<MipMap>()));
  }

  Producer producer(params.getInt32("num"), params.getInt32("minvalue"));
  Sink sink;

  ff_Farm<MipMapTask, std::string> farm(std::move(W), producer, sink);

  ff_Pipe<> pipe(farm);

  if (pipe.run_and_wait_end() < 0)
    error("running pipe\n");
}
