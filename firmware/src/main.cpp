#include <Arduino.h>
#include <LittleFS.h>
#include <WiFi.h>
#include <ESPAsyncWebServer.h>

#include "actuators/RelayController.h"
#include "config/AppConfig.h"
#include "mqtt/MqttModule.h"
#include "net/ApiServer.h"
#include "sensors/SensorManager.h"
#include "storage/StateStore.h"

namespace {
AsyncWebServer gServer(AppConfig::HTTP_PORT);
StateStore gStateStore;
RelayController gRelayController;
SensorManager gSensorManager(AppConfig::DHT_PIN, AppConfig::DHT_TYPE, AppConfig::PIR_PIN);
ApiServer gApiServer(gServer, gRelayController, gSensorManager);
MqttModule gMqtt;

void connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.begin(AppConfig::SSID, AppConfig::PASSWORD);

  Serial.print("[WiFi] Connecting");
  const uint32_t startMs = millis();

  while (WiFi.status() != WL_CONNECTED && (millis() - startMs) < 15000) {
    delay(250);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println();
    Serial.print("[WiFi] Connected, IP: ");
    Serial.println(WiFi.localIP());
    return;
  }

  Serial.println();
  Serial.println("[WiFi] STA failed, starting AP fallback");
  WiFi.mode(WIFI_AP_STA);
  WiFi.softAP(AppConfig::AP_SSID, AppConfig::AP_PASSWORD);
  Serial.print("[WiFi] AP IP: ");
  Serial.println(WiFi.softAPIP());
}
}  // namespace

void setup() {
  Serial.begin(115200);
  delay(200);

  if (!LittleFS.begin(true)) {
    Serial.println("[FS] Failed to mount LittleFS");
  }

  if (!gStateStore.begin()) {
    Serial.println("[Store] Preferences init failed");
  }

  gRelayController.begin(gStateStore.loadRelayState());
  gSensorManager.begin();

  gRelayController.setStateChangedCallback([](const RelayState& state) {
    gStateStore.saveRelayState(state);
    gApiServer.notifyRelayChanged(state);
    gMqtt.publishRelayState(state);
  });

  gSensorManager.setSnapshotChangedCallback([](const SensorSnapshot& snapshot) {
    gApiServer.notifySensorChanged(snapshot);
    gMqtt.publishSensorSnapshot(snapshot);
  });

  gMqtt.setRelayCommandHandler([](size_t channel, bool on) {
    gRelayController.setChannel(channel, on);
  });

  connectWiFi();

  gApiServer.begin();
  gMqtt.begin();

  gApiServer.notifyRelayChanged(gRelayController.state());
  gApiServer.notifySensorChanged(gSensorManager.snapshot());

  Serial.println("[System] Smart Home controller started");
}

void loop() {
  gSensorManager.tick(millis());
  gMqtt.tick();
  delay(5);
}
