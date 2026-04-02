#pragma once

#include <ESPAsyncWebServer.h>

#include "actuators/RelayController.h"
#include "sensors/SensorManager.h"

class ApiServer {
 public:
  ApiServer(AsyncWebServer& server, RelayController& relayController, SensorManager& sensorManager);

  void begin();
  void notifyRelayChanged(const RelayState& relayState);
  void notifySensorChanged(const SensorSnapshot& sensorSnapshot);

 private:
  void setupRoutes();
  void sendSystemSnapshot(AsyncWebServerRequest* request);

  AsyncWebServer& server_;
  RelayController& relayController_;
  SensorManager& sensorManager_;
  AsyncEventSource events_;
};
