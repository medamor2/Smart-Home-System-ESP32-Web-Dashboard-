#pragma once

#include <ArduinoJson.h>

#include "core/Models.h"

namespace JsonCodec {

String relayStateToJson(const RelayState& state);
String sensorSnapshotToJson(const SensorSnapshot& snapshot);
String systemSnapshotToJson(const RelayState& relays, const SensorSnapshot& sensors);

}  // namespace JsonCodec
