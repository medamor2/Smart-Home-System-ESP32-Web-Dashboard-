#include "actuators/RelayController.h"

#include "config/AppConfig.h"

void RelayController::begin(const RelayState& initialState) {
  state_ = initialState;

  for (size_t i = 0; i < 4; ++i) {
    pinMode(AppConfig::RELAY_PINS[i], OUTPUT);
    writeHardware(i, state_.channels[i]);
  }
}

bool RelayController::setChannel(size_t channel, bool on) {
  if (channel >= 4) {
    return false;
  }

  if (state_.channels[channel] == on) {
    return true;
  }

  state_.channels[channel] = on;
  writeHardware(channel, on);

  if (onStateChanged_) {
    onStateChanged_(state_);
  }

  return true;
}

bool RelayController::toggleChannel(size_t channel) {
  if (channel >= 4) {
    return false;
  }

  return setChannel(channel, !state_.channels[channel]);
}

RelayState RelayController::state() const {
  return state_;
}

void RelayController::setStateChangedCallback(OnStateChanged cb) {
  onStateChanged_ = cb;
}

void RelayController::writeHardware(size_t channel, bool on) {
  const uint8_t activeLevel = AppConfig::RELAY_ACTIVE_LOW ? LOW : HIGH;
  const uint8_t inactiveLevel = AppConfig::RELAY_ACTIVE_LOW ? HIGH : LOW;

  digitalWrite(AppConfig::RELAY_PINS[channel], on ? activeLevel : inactiveLevel);
}
