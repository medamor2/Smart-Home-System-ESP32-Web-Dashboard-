#pragma once

#include <Arduino.h>
#include <functional>

#include "core/Models.h"

class MqttModule {
 public:
  using RelayCommandHandler = std::function<void(size_t channel, bool on)>;

  void begin();
  void tick();
  void publishRelayState(const RelayState& state);
  void publishSensorSnapshot(const SensorSnapshot& snapshot);
  void setRelayCommandHandler(RelayCommandHandler handler);

 private:
  RelayCommandHandler relayCommandHandler_;
};
