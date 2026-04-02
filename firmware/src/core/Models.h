#pragma once

#include <Arduino.h>

struct RelayState {
  bool channels[4] = {false, false, false, false};
};

struct SensorSnapshot {
  float temperatureC = NAN;
  bool motionDetected = false;
  uint32_t sampledAtMs = 0;
};
