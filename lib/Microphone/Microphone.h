#ifndef MICROPHONE_H
#define MICROPHONE_H

#include <Arduino.h>
#include "driver/gpio.h" // Aggiungi questo per il controllo diretto

// Pin analogico per il MAX4466
#define MIC_PIN 35 

// Valore attuale del livello sonoro (Peak-to-Peak)
extern volatile float currentSoundLevel; 

void Microphone_Init();
void Microphone_Task(void* pvParameters);

#endif