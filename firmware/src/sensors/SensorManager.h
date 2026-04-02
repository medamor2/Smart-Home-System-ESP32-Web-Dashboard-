#pragma once

#include <Arduino.h>
#include <DHT.h>
#include <functional>

#include "core/Models.h"

class SensorManager {
 public:
  using OnSnapshotChanged = std::function<void(const SensorSnapshot&)>;

  SensorManager(uint8_t dhtPin, uint8_t dhtType, uint8_t pirPin);

  void begin();
  void tick(uint32_t nowMs);
  SensorSnapshot snapshot() const;
  void setSnapshotChangedCallback(OnSnapshotChanged cb);

 private:
  bool publishIfChanged(const SensorSnapshot& candidate);

  DHT dht_;
  uint8_t pirPin_;
  SensorSnapshot snapshot_;
  OnSnapshotChanged onSnapshotChanged_;
  uint32_t lastReadMs_ = 0;
};
