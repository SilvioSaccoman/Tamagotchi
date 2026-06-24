#include <Arduino.h>
#include "Display.h"
#include "CoreStats.h"
#include "Sprites.h"
#include "Memory.h"
#include "Microphone.h"
#include "LightSensor.h"
#include "Gyro.h"

TFT_eSPI tft = TFT_eSPI();

// Initialization of the stats and state
struct Stats stats = {
    .hungerLevel = 100,
    .healthLevel = 100,
    .energyLevel = 100,
    .happinessLevel = 100,
    .life_seconds= 0,
    .total_steps = 0
};

struct State currentState = {
    .evolution = ELDER,
    .hungerLevel = NOT_HUNGRY,
    .healthLevel = HEALTHY,
    .energyLevel = ENERGETIC,
    .happinessLevel = UNHAPPY
};

#define BUTTON_RESET_PIN 0 // Often the 'BOOT' button on ESP32 boards

extern "C" void app_main() {

    // Initialization 
    initArduino(); // Initialize Arduino framework 
    esp_log_level_set("gpio", ESP_LOG_NONE);
    
    //loadStats();   // Load stats from memory
    // if (currentState.evolution == DEAD) {
    //     loadDefaultStats(); // If dead, reset to default stats
    // }
    Display_init(); // Initialize the display

    
    // Create tasks
    xTaskCreate(StatsUpdate_Task, "StatsUpdate_Task", 4096, NULL, 1, NULL);
    xTaskCreate(Microphone_Task, "Microphone_Task", 4096, NULL, 1, NULL);
    xTaskCreate(LightSensor_Task, "LightSensor_Task", 4096, NULL, 1, NULL);
    xTaskCreate(Gyroscope_Task, "Gyroscope_Task", 4096, NULL, 1, NULL);
    xTaskCreatePinnedToCore(DisplayUpdate_Task, "DisplayUpdate_Task", 4096, NULL, 1, NULL, 1); // Run the display task on core 1 to avoid conflicts with the stats update task
    
    // Main loop
    while(1) {

        ESP_LOGI("STATS", "Current_stats: Hunger: %d, Health: %d, Energy: %d, Happiness: %d, Evolution: %d", 
                stats.hungerLevel, stats.healthLevel, stats.energyLevel, stats.happinessLevel, currentState.evolution);

    //    ESP_LOGI("MICROPHONE", "Mic Level: %.2f\n", currentSoundLevel);

        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}