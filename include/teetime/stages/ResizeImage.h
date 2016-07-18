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
#include <teetime/stages/FunctionStage.h>
#include <teetime/Image.h>

namespace teetime
{
  struct ImageToResize
  {
    Image image;
    size_t width;
    size_t height;
  };

  inline Image resizeImage(ImageToResize& image)
  {
    return image.image.resize(image.width, image.height);
  }

  using ResizeImage = FunctionStage<const ImageToResize&, Image, resizeImage>;
}