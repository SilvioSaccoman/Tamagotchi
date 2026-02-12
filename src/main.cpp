#include <Arduino.h>
#include "Display.h"
#include "CoreStats.h"
#include "Sprites.h"

TFT_eSPI tft = TFT_eSPI();

// Initialization of the stats and state
struct Stats stats = {
    .hungerLevel = 100,
    .healthLevel = 100,
    .energyLevel = 100,
    .happinessLevel = 100,
    .life_seconds= 0
};

struct State currentState = {
    .evolution = EGG,
    .hungerLevel = NOT_HUNGRY,
    .healthLevel = HEALTHY,
    .energyLevel = ENERGETIC,
    .happinessLevel = UNHAPPY
};

extern "C" void app_main() {
    initArduino();
    Display_init();    

    xTaskCreate(StatsUpdate_Task, "StatsUpdate_Task", 4096, NULL, 1, NULL);
    xTaskCreatePinnedToCore(DisplayUpdate_Task, "DisplayUpdate_Task", 4096, NULL, 1, NULL, 1); // Run the display task on core 1 to avoid conflicts with the stats update task
    
    while(1) {
        ESP_LOGI("STATS", "Current_stats: Hunger: %d, Health: %d, Energy: %d, Happiness: %d, Evolution: %d", 
                stats.hungerLevel, stats.healthLevel, stats.energyLevel, stats.happinessLevel, currentState.evolution);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}