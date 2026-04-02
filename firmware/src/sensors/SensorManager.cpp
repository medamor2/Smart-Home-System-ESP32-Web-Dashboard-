#include "sensors/SensorManager.h"

#include <cmath>

#include "config/AppConfig.h"

SensorManager::SensorManager(uint8_t dhtPin, uint8_t dhtType, uint8_t pirPin)
    : dht_(dhtPin, dhtType), pirPin_(pirPin) {}

void SensorManager::begin() {
  pinMode(pirPin_, INPUT);
  dht_.begin();

  snapshot_.motionDetected = digitalRead(pirPin_) == HIGH;
  snapshot_.temperatureC = dht_.readTemperature();
  snapshot_.sampledAtMs = millis();
  lastReadMs_ = 0;
}

void SensorManager::tick(uint32_t nowMs) {
  if (nowMs - lastReadMs_ < AppConfig::SENSOR_PUBLISH_INTERVAL_MS) {
    return;
  }

  lastReadMs_ = nowMs;

  SensorSnapshot candidate = snapshot_;
  candidate.motionDetected = digitalRead(pirPin_) == HIGH;

  const float t = dht_.readTemperature();
  if (!isnan(t)) {
    candidate.temperatureC = t;
  }
  candidate.sampledAtMs = nowMs;

  publishIfChanged(candidate);
}

SensorSnapshot SensorManager::snapshot() const {
  return snapshot_;
}

void SensorManager::setSnapshotChangedCallback(OnSnapshotChanged cb) {
  onSnapshotChanged_ = cb;
}

bool SensorManager::publishIfChanged(const SensorSnapshot& candidate) {
  const bool changed =
      (candidate.motionDetected != snapshot_.motionDetected) ||
      (isnan(snapshot_.temperatureC) != isnan(candidate.temperatureC)) ||
      (!isnan(snapshot_.temperatureC) && !isnan(candidate.temperatureC) &&
       fabs(candidate.temperatureC - snapshot_.temperatureC) >= 0.1f);

  snapshot_ = candidate;

  if (changed && onSnapshotChanged_) {
    onSnapshotChanged_(snapshot_);
  }

  return changed;
}
