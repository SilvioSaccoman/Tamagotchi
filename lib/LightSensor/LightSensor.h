#ifndef LIGHT_SENSOR_H
#define LIGHT_SENSOR_H

#include "Activities.h"
#include "Stats.h"
#include <Arduino.h>

#define LDR_PIN 34 
#define LIGHT_THRESHOLD 200 // Threshold for determining if it's dark or bright
#define LIGHT_TIME_THRESHOLD 3// Time threshold in seconds to consider it dark for sleeping

extern int lightLevel; // Variable to hold the current light level, updated by the LightSensor_Task

void LightSensor_Task(void* pvParameters);

#endif // LIGHT_SENSOR_H