#pragma once

#include <Arduino.h>
#include <Preferences.h>

#include "core/Models.h"

class StateStore {
 public:
  bool begin();
  RelayState loadRelayState();
  bool saveRelayState(const RelayState& state);

 private:
  Preferences prefs_;
};
