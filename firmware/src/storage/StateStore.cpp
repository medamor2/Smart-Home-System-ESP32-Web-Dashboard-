#include "storage/StateStore.h"

#include "config/AppConfig.h"

bool StateStore::begin() {
  return prefs_.begin(AppConfig::PREF_NAMESPACE, false);
}

RelayState StateStore::loadRelayState() {
  RelayState state;
  uint8_t mask = prefs_.getUChar(AppConfig::PREF_RELAY_KEY, 0);

  for (size_t i = 0; i < 4; ++i) {
    state.channels[i] = (mask >> i) & 0x01;
  }

  return state;
}

bool StateStore::saveRelayState(const RelayState& state) {
  uint8_t mask = 0;
  for (size_t i = 0; i < 4; ++i) {
    if (state.channels[i]) {
      mask |= static_cast<uint8_t>(1 << i);
    }
  }

  return prefs_.putUChar(AppConfig::PREF_RELAY_KEY, mask) > 0;
}
