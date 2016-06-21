#pragma once

namespace teetime
{
  enum class SignalType
  {
    None,
    Start,
    Terminating
  };

  struct Signal
  {
    SignalType type;
  };

  inline const char* toString(SignalType type)
  {
    switch(type)
    {
    case SignalType::Start:
      return "SignalType::Start";
    case SignalType::Terminating:
      return "SignalType::Terminating";      
    default:
      break;
    }

    return "SignalType::?";      
  }
}