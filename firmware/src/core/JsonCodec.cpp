#include "core/JsonCodec.h"

namespace JsonCodec {

String relayStateToJson(const RelayState& state) {
  JsonDocument doc;
  JsonArray channels = doc["channels"].to<JsonArray>();
  for (size_t i = 0; i < 4; ++i) {
    channels.add(state.channels[i]);
  }

  String payload;
  serializeJson(doc, payload);
  return payload;
}

String sensorSnapshotToJson(const SensorSnapshot& snapshot) {
  JsonDocument doc;
  if (isnan(snapshot.temperatureC)) {
    doc["temperatureC"] = nullptr;
  } else {
    doc["temperatureC"] = snapshot.temperatureC;
  }
  doc["motionDetected"] = snapshot.motionDetected;
  doc["sampledAtMs"] = snapshot.sampledAtMs;

  String payload;
  serializeJson(doc, payload);
  return payload;
}

String systemSnapshotToJson(const RelayState& relays, const SensorSnapshot& sensors) {
  JsonDocument doc;

  JsonArray channels = doc["relays"]["channels"].to<JsonArray>();
  for (size_t i = 0; i < 4; ++i) {
    channels.add(relays.channels[i]);
  }

  if (isnan(sensors.temperatureC)) {
    doc["sensors"]["temperatureC"] = nullptr;
  } else {
    doc["sensors"]["temperatureC"] = sensors.temperatureC;
  }
  doc["sensors"]["motionDetected"] = sensors.motionDetected;
  doc["sensors"]["sampledAtMs"] = sensors.sampledAtMs;

  String payload;
  serializeJson(doc, payload);
  return payload;
}

}  // namespace JsonCodec
