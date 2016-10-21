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

namespace teetime
{
  class AbstractStage;

  class Runnable
  {
  public:
    Runnable();
    virtual ~Runnable() = default;
    virtual void run() = 0;

  protected:
    uint64 creationTime;
  };

  class AbstractStageRunnable : public Runnable
  {
  protected:
    explicit AbstractStageRunnable(AbstractStage* stage);
    AbstractStage* m_stage;
  };

  class ProducerStageRunnable final : public AbstractStageRunnable
  {
  public:

    explicit ProducerStageRunnable(AbstractStage* stage);
    virtual void run() override;
  };

  class ConsumerStageRunnable final : public AbstractStageRunnable
  {
  public:
    explicit ConsumerStageRunnable(AbstractStage* stage);
    virtual void run() override;
  }; 
}