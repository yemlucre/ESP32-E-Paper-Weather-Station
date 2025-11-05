#ifndef DHT_SENSOR_H
#define DHT_SENSOR_H

#include "DHT.h"

struct SensorData {
  float temperature;
  float humidity;
  bool valid;
};

void initDHT(uint8_t pin);
SensorData readDHT();

#endif
