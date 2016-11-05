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
#include "AbstractPipe.h"

namespace teetime
{
  template<typename T> class Optional;
  
  /**
   * Pipe interface
   * @tparam T type of elements to be passed though this pipe.
   */
  template<typename T>
  class Pipe : public AbstractPipe
  {
  public:    
    virtual ~Pipe() = default;

    /**
     * remove last element from pipe.
     * @return optional that contains either the last value, or is empty if pipe was also empty.
     */
    virtual Optional<T> removeLast() = 0;

    /**
     * Add an element to the queue. If queue is full, this blocks until the element was added
     * @param t element to add
     */
    virtual void add(T&& t) = 0;

    /**
     * Try to add an element to the queue. If queue is full, returns false. Does not block.
     * @param t element to add
     * @return true if element was added, false otherwise.
     */
    virtual bool tryAdd(T&& t) = 0;


    virtual bool isEmpty() const = 0;
  };
}