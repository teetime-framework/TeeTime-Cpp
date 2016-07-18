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
#include <teetime/Image.h>
#include <mutex>

#pragma warning( push )
#pragma warning( disable: 4244 )
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STB_IMAGE_RESIZE_IMPLEMENTATION
#include "stb_image_resize.h"
#pragma warning( pop )
using namespace teetime;


static void init_stb()
{
  static bool done = false;
  static std::mutex mutex;

  if (!done)
  {
    mutex.lock();
    if (!done)
    {
      stbi__init_zdefaults();
      done = true;
    }
    mutex.unlock();
  }
}

Image::Image()
  : m_width(0)
  , m_height(0)
  , m_data(nullptr)
{}

Image::Image(const Image& rhs)
  : m_width(rhs.m_width)
  , m_height(rhs.m_height)
  , m_data((Rgba*)::malloc(rhs.m_width * rhs.m_height * sizeof(Image::Rgba)))
  , m_filename(rhs.m_filename)
{
  memcpy(m_data, rhs.m_data, rhs.m_width * rhs.m_height * sizeof(Image::Rgba));
}

Image::Image(Image&& rhs)
  : m_width(rhs.m_width)
  , m_height(rhs.m_height)
  , m_data(rhs.m_data)
  , m_filename(std::move(rhs.m_filename))
{
  rhs.m_width = 0;
  rhs.m_height = 0;
  rhs.m_data = nullptr;
}

Image::~Image()
{
  reset();
}

const Image& Image::operator =(const Image& rhs)
{
  reset();

  m_width = rhs.m_width;
  m_height = rhs.m_height;
  m_data = (Rgba*)::malloc(rhs.m_width * rhs.m_height * sizeof(Image::Rgba));
  m_filename = rhs.m_filename;

  memcpy(m_data, rhs.m_data, rhs.m_width * rhs.m_height * sizeof(Image::Rgba));

  return *this;
}

const Image& Image::operator =(Image&& rhs)
{
  reset();

  m_width = rhs.m_width;
  m_height = rhs.m_height;
  m_data = rhs.m_data;
  m_filename = std::move(rhs.m_filename);

  rhs.m_width = 0;
  rhs.m_height = 0;
  rhs.m_data = nullptr;  

  return *this;
}

void Image::reset()
{
  if (m_data)
  {
    stbi_image_free(m_data);    
  }

  m_data = nullptr;
  m_height = 0;
  m_width = 0;
  m_filename = "";
}

bool Image::loadFromFile(const std::string& filename)
{
  reset();

  init_stb();
  int width = 0;
  int height = 0;
  int comp = 0;  

  if (auto p = stbi_load(filename.c_str(), &width, &height, &comp, 4))
  {
    assert(width > 0);
    assert(height > 0);
    assert(comp > 0);

    m_data = (Image::Rgba*)p;
    m_width = width;
    m_height = height;
    m_filename = filename;

    return true;
  }

  return false;
}

bool Image::loadFromMemory(const uint8* data, size_t dataSize, const char* filename)
{
  reset();

  init_stb();
  int width = 0;
  int height = 0;
  int comp = 0;
  
  if (auto p = stbi_load_from_memory(data, dataSize, &width, &height, &comp, 4))
  {
    assert(width > 0);
    assert(height > 0);
    assert(comp > 0);

    m_data = (Image::Rgba*)p;
    m_width = width;
    m_height = height;
    m_filename = filename;

    return true;
  }

  return false;
}

Image Image::resize(size_t width, size_t height) const
{
  return *this;
}