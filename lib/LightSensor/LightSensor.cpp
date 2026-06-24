#include "LightSensor.h"
#include "CoreStats.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"

int lightLevel = 0; // Variable to hold the current light level, updated by the LightSensor_Task
extern struct Stats stats;

// Riferimento al Mutex allocato nel modulo del microfono
extern SemaphoreHandle_t adcMutex;

void LightSensor_Task(void* pvParameters) {
    
    // Sicurezza: se questo task parte prima del microfono, crea lui il Mutex
    if (adcMutex == NULL) {
        adcMutex = xSemaphoreCreateMutex();
    }

    int time_light_level = 0;
    int time_dark_level = 0;

    while (1) {
        
        // Protezione dell'ADC tramite Mutex
        if (xSemaphoreTake(adcMutex, portMAX_DELAY) == pdTRUE) {
            lightLevel = analogRead(LDR_PIN); // Read the light level from the LDR sensor
            xSemaphoreGive(adcMutex);         // Rilascia immediatamente l'ADC
        }

        ESP_LOGI("LIGHT_SENSOR", "Light Level: %d\n", lightLevel);

        if (lightLevel > LIGHT_THRESHOLD) { // ------------------ BUIO
            time_light_level = 0; // Se è buio, resetta SEMPRE il timer della luce
            
            if (!isSleeping) {
                time_dark_level++;
                
                // Controlla il timeout del buio
                if (time_dark_level >= LIGHT_TIME_THRESHOLD) {
                    // Manda a dormire solo se è effettivamente stanco (<= 40)
                    if (stats.energyLevel <= 40) {
                        ESP_LOGI("LIGHT_SENSOR", "Buio e stanco (Energia: %d)! Vado a dormire...", stats.energyLevel);
                        Sleeping(&stats);
                    } else {
                        ESP_LOGI("LIGHT_SENSOR", "E' buio, ma ho ancora troppa energia (%d) per dormire.", stats.energyLevel);
                    }
                    time_dark_level = 0; // Resetta il timer in ogni caso dopo la scadenza
                }
            } else {
                time_dark_level = 0; // Già dorme, azzera il timer del buio
            }

        } else { // --------------------------------------------- LUCE
            time_dark_level = 0; // Se c'è luce, resetta SEMPRE il timer del buio
            
            if (isSleeping) {
                time_light_level++;
                
                // Controlla il timeout della luce
                if (time_light_level >= LIGHT_TIME_THRESHOLD) {
                    // Si sveglia con la luce solo se ha recuperato abbastanza energia (>= 80)
                    if (stats.energyLevel >= 80) {
                        ESP_LOGI("LIGHT_SENSOR", "Luce e riposato (Energia: %d)! Sveglia!", stats.energyLevel);
                        isSleeping = false;
                    } else {
                        ESP_LOGI("LIGHT_SENSOR", "C'e' luce, ma sono ancora stanco (%d). Continuo a dormire...", stats.energyLevel);
                    }
                    time_light_level = 0; // Resetta il timer in ogni caso dopo la scadenza
                }
            } else {
                time_light_level = 0; // Già sveglio, azzera il timer della luce
            }
        }

        vTaskDelay(pdMS_TO_TICKS(1000)); // Delay for 1 second before reading again
    }
}