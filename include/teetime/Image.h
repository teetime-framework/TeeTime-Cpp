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
#pragma once
#include "common.h"
#include <string>

namespace teetime
{
  class Image
  {
  public:
    struct Rgba
    {
      uint8 r;
      uint8 g;
      uint8 b;
      uint8 a;
    };

    Image();
    Image(const Image& rhs);
    Image(Image&& rhs);

    ~Image();

    const Image& operator=(const Image& rhs);
    const Image& operator=(Image&& rhs);

    size_t getWidth() const;
    size_t getHeight() const;

    const Rgba* getRgba() const;
    Rgba*       getRgba();

    std::string getFilename() const;

    bool loadFromFile(const std::string& filename);
    bool loadFromMemory(const uint8* data, size_t dataSize, const char* filename);

    void reset();

    Image resize(size_t width, size_t height) const;

  private:
    size_t m_width;
    size_t m_height;
    Rgba* m_data;
    std::string m_filename;
  };

  inline size_t Image::getWidth() const
  {
    return m_width;
  }

  inline size_t Image::getHeight() const
  {
    return m_height;
  }

  inline const Image::Rgba* Image::getRgba() const
  {
    return m_data;
  }

  inline Image::Rgba* Image::getRgba()
  {
    return m_data;
  }

  inline std::string Image::getFilename() const
  {
    return m_filename;
  }
}