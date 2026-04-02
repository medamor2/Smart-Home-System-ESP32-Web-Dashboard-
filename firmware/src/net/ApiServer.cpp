#include "net/ApiServer.h"

#include <LittleFS.h>

#include "core/JsonCodec.h"

ApiServer::ApiServer(AsyncWebServer& server, RelayController& relayController,
                     SensorManager& sensorManager)
    : server_(server), relayController_(relayController), sensorManager_(sensorManager), events_("/events") {}

void ApiServer::begin() {
  setupRoutes();

  events_.onConnect([this](AsyncEventSourceClient* client) {
    client->send(JsonCodec::systemSnapshotToJson(relayController_.state(), sensorManager_.snapshot()).c_str(),
                 "system", millis());
  });

  server_.addHandler(&events_);
  server_.begin();
}

void ApiServer::notifyRelayChanged(const RelayState& relayState) {
  events_.send(JsonCodec::relayStateToJson(relayState).c_str(), "relays", millis());
  events_.send(JsonCodec::systemSnapshotToJson(relayState, sensorManager_.snapshot()).c_str(), "system", millis());
}

void ApiServer::notifySensorChanged(const SensorSnapshot& sensorSnapshot) {
  events_.send(JsonCodec::sensorSnapshotToJson(sensorSnapshot).c_str(), "sensors", millis());
  events_.send(JsonCodec::systemSnapshotToJson(relayController_.state(), sensorSnapshot).c_str(), "system", millis());
}

void ApiServer::setupRoutes() {
  server_.serveStatic("/", LittleFS, "/").setDefaultFile("index.html");

  server_.on("/api/health", HTTP_GET, [](AsyncWebServerRequest* request) {
    request->send(200, "application/json", "{\"status\":\"ok\"}");
  });

  server_.on("/api/state", HTTP_GET, [this](AsyncWebServerRequest* request) { sendSystemSnapshot(request); });

  server_.on("/api/relays", HTTP_GET, [this](AsyncWebServerRequest* request) {
    request->send(200, "application/json", JsonCodec::relayStateToJson(relayController_.state()));
  });

  server_.on("/api/sensors", HTTP_GET, [this](AsyncWebServerRequest* request) {
    request->send(200, "application/json", JsonCodec::sensorSnapshotToJson(sensorManager_.snapshot()));
  });

  for (size_t i = 0; i < 4; ++i) {
    const String setRoute = "/api/relays/" + String(i);
    server_.on(
        setRoute.c_str(), HTTP_POST,
        [](AsyncWebServerRequest*) {},
        nullptr,
        [this, i](AsyncWebServerRequest* request, uint8_t* data, size_t len, size_t index, size_t total) {
          if (index != 0 || len != total) {
            request->send(400, "application/json", "{\"error\":\"Body must be provided in one chunk\"}");
            return;
          }

          JsonDocument doc;
          const auto err = deserializeJson(doc, data, len);
          if (err || !doc["on"].is<bool>()) {
            request->send(400, "application/json", "{\"error\":\"Expected JSON {\\\"on\\\":true|false}\"}");
            return;
          }

          relayController_.setChannel(i, doc["on"].as<bool>());
          request->send(200, "application/json", JsonCodec::relayStateToJson(relayController_.state()));
        });

    const String toggleRoute = "/api/relays/" + String(i) + "/toggle";
    server_.on(toggleRoute.c_str(), HTTP_POST, [this, i](AsyncWebServerRequest* request) {
      relayController_.toggleChannel(i);
      request->send(200, "application/json", JsonCodec::relayStateToJson(relayController_.state()));
    });
  }

  server_.onNotFound([](AsyncWebServerRequest* request) {
    request->send(404, "application/json", "{\"error\":\"Not found\"}");
  });
}

void ApiServer::sendSystemSnapshot(AsyncWebServerRequest* request) {
  request->send(200, "application/json",
                JsonCodec::systemSnapshotToJson(relayController_.state(), sensorManager_.snapshot()));
}
