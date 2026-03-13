#ifndef MICROPHONE_H
#define MICROPHONE_H

#include <Arduino.h>
#include <driver/i2s.h>

// Definiamo i pin basandoci sui connettori CN1/P3 del CYD
#define I2S_WS      21   // Word Select
#define I2S_SD      35   // Serial Data
#define I2S_SCK     22   // Serial Clock
#define I2S_PORT    I2S_NUM_0

extern volatile float currentSoundLevel; // Valore attuale del livello sonoro, aggiornato dal task del microfono

void Microphone_Init();
void Microphone_Task(void* pvParameters);


#endif