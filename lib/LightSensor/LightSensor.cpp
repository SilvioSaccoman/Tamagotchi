#include "LightSensor.h"

int lightLevel = 0; // Variable to hold the current light level, updated by the LightSensor_Task
extern struct Stats stats;

void LightSensor_Task(void* pvParameters) {
    
    int time_light_level = 0;
    int time_dark_level = 0;

     while (1) {
        pinMode(LDR_PIN, ANALOG); // Analog mode for the LDR pin
        lightLevel = analogRead(LDR_PIN); // Read the light level from the LDR sensor

        ESP_LOGI("LIGHT_SENSOR", "Light Level: %d\n", lightLevel);

        if (lightLevel > LIGHT_THRESHOLD) { // Higher = Darker 
            time_light_level = 0; // Reset light level timer
            time_dark_level ++; // Increment dark level timer
            if (time_dark_level >= LIGHT_TIME_THRESHOLD && !isSleeping) { // If it's been dark for long enough, go to sleep
                ESP_LOGI("LIGHT_SENSOR", "It's been dark for %d seconds! Going to sleep...", time_dark_level);
                Sleeping(&stats); // Trigger sleeping activity
                time_dark_level = 0; // Reset dark level timer
            }
        } else {
            time_dark_level = 0; // Reset dark level timer
            time_light_level += 1; // Increment light level timer
            if (time_light_level >= LIGHT_TIME_THRESHOLD && isSleeping) { // If it's been light for long enough, wake up
                ESP_LOGI("LIGHT_SENSOR", "It's been bright for %d seconds! Waking up...", time_light_level);
                isSleeping = false; // Wake up the Tamagotchi
                time_light_level = 0; // Reset light level timer
            }
        }

        vTaskDelay(pdMS_TO_TICKS(1000)); // Delay for 1 second before reading again
    }
}