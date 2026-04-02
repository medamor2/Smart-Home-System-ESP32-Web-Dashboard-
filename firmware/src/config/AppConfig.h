#pragma once

#include <Arduino.h>

namespace AppConfig {

constexpr const char* SSID = "SSID";
constexpr const char* PASSWORD = "PASSWORD";

constexpr const char* AP_SSID = "Moncef-SmartHome";
constexpr const char* AP_PASSWORD = "change-me-123";

constexpr uint16_t HTTP_PORT = 80;
constexpr uint32_t SENSOR_PUBLISH_INTERVAL_MS = 2000;

constexpr uint8_t RELAY_PINS[4] = {16, 17, 18, 19};
constexpr bool RELAY_ACTIVE_LOW = true;

constexpr uint8_t DHT_PIN = 4;
constexpr uint8_t PIR_PIN = 27;

constexpr uint8_t DHT_TYPE = 22;

constexpr const char* PREF_NAMESPACE = "smarthome";
constexpr const char* PREF_RELAY_KEY = "relaymask";

constexpr const char* MQTT_HOST = "192.168.1.10";
constexpr uint16_t MQTT_PORT = 1883;
constexpr const char* MQTT_CLIENT_ID = "esp32-smarthome";
constexpr const char* MQTT_USER = "";
constexpr const char* MQTT_PASS = "";
constexpr const char* MQTT_BASE_TOPIC = "home/esp32";

}  // namespace AppConfig
