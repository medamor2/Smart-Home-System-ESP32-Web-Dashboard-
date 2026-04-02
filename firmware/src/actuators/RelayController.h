#pragma once

#include <Arduino.h>
#include <functional>

#include "core/Models.h"

class RelayController {
 public:
  using OnStateChanged = std::function<void(const RelayState&)>;

  void begin(const RelayState& initialState);
  bool setChannel(size_t channel, bool on);
  bool toggleChannel(size_t channel);
  RelayState state() const;
  void setStateChangedCallback(OnStateChanged cb);

 private:
  void writeHardware(size_t channel, bool on);

  RelayState state_;
  OnStateChanged onStateChanged_;
};
