#pragma once

namespace teetime
{
  class AbstractStage;

  class Runnable
  {
  public:
    virtual ~Runnable() = default;
    virtual void run() = 0;
  };

  class AbstractStageRunnable : public Runnable
  {
  protected:
    explicit AbstractStageRunnable(AbstractStage* stage);
    AbstractStage* m_stage;
  };

  class ProducerStageRunnable : public AbstractStageRunnable
  {
  public:

    explicit ProducerStageRunnable(AbstractStage* stage);
    virtual void run() override;
  };

  class ConsumerStageRunnable : public AbstractStageRunnable
  {
  public:
    explicit ConsumerStageRunnable(AbstractStage* stage);

    virtual void run() override;
  }; 
}