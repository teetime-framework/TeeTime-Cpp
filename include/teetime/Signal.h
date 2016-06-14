#pragma once

namespace teetime
{
  enum class SignalType
  {
    Start
  };

  struct Signal
  {
    SignalType type;
  };
}