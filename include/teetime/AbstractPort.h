#pragma once
#include "teetime/common.h"

namespace teetime
{
  class AbstractStage;

  class AbstractPort
  {
  public:
    virtual ~AbstractPort() = default;

    const AbstractStage* owner() const
    {
      return m_owner;
    }

    AbstractStage* owner()
    {
      return m_owner;
    }    

  protected:
    explicit AbstractPort(AbstractStage* owner)
     : m_owner(owner)
    {   
      assert(owner);   
    }

  private:
    AbstractStage* m_owner;
  };
}