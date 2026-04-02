#include "mqtt/MqttModule.h"

#include "config/AppConfig.h"

#if ENABLE_MQTT

#include <PubSubClient.h>
#include <WiFi.h>

#include "core/JsonCodec.h"

namespace {
WiFiClient gWifiClient;
PubSubClient gClient(gWifiClient);
MqttModule::RelayCommandHandler gRelayCommandHandler;

String topicFor(const String& suffix) {
  return String(AppConfig::MQTT_BASE_TOPIC) + "/" + suffix;
}

void onMqttMessage(char* topic, uint8_t* payload, unsigned int length) {
  String topicStr(topic);
  String body;
  body.reserve(length);
  for (unsigned int i = 0; i < length; ++i) {
    body += static_cast<char>(payload[i]);
  }

  const String prefix = topicFor("cmd/relay/");
  if (!topicStr.startsWith(prefix)) {
    return;
  }

  const size_t channel = topicStr.substring(prefix.length()).toInt();
  const bool on = body == "1" || body == "on" || body == "true";

  if (gRelayCommandHandler) {
    gRelayCommandHandler(channel, on);
  }
}
}  // namespace

void MqttModule::begin() {
  gClient.setServer(AppConfig::MQTT_HOST, AppConfig::MQTT_PORT);
  gClient.setCallback(onMqttMessage);
  gRelayCommandHandler = relayCommandHandler_;
}

void MqttModule::tick() {
  if (!gClient.connected()) {
    const bool connected = gClient.connect(AppConfig::MQTT_CLIENT_ID, AppConfig::MQTT_USER, AppConfig::MQTT_PASS);
    if (connected) {
      gClient.subscribe(topicFor("cmd/relay/+" ).c_str());
    }
  }

  gClient.loop();
}

void MqttModule::publishRelayState(const RelayState& state) {
  if (!gClient.connected()) {
    return;
  }

  gClient.publish(topicFor("state/relays").c_str(), JsonCodec::relayStateToJson(state).c_str(), true);
}

void MqttModule::publishSensorSnapshot(const SensorSnapshot& snapshot) {
  if (!gClient.connected()) {
    return;
  }

  gClient.publish(topicFor("state/sensors").c_str(), JsonCodec::sensorSnapshotToJson(snapshot).c_str(), true);
}

void MqttModule::setRelayCommandHandler(RelayCommandHandler handler) {
  relayCommandHandler_ = handler;
  gRelayCommandHandler = handler;
}

#else

void MqttModule::begin() {}
void MqttModule::tick() {}
void MqttModule::publishRelayState(const RelayState&) {}
void MqttModule::publishSensorSnapshot(const SensorSnapshot&) {}
void MqttModule::setRelayCommandHandler(RelayCommandHandler handler) { relayCommandHandler_ = handler; }

#endif
