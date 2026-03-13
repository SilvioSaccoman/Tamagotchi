#ifndef GYROSCOPE_H
#define GYROSCOPE_H

#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

// Definiamo i pin che abbiamo scelto
#define I2C_SDA 27
#define I2C_SCL 22

void Gyroscope_Init();
void Gyroscope_Task(void* pvParameters);

extern float gyroX, gyroY, gyroZ; // Variabili globali per il movimento

#endif // GYROSCOPE_H