#pragma once

namespace teetime
{
  enum class SignalType
  {
    Start,
    Terminating
  };

  struct Signal
  {
    SignalType type;
  };
}