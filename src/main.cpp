#include <Arduino.h>
#include <TFT_eSPI.h>
#include "CoreStats.h"


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
    
    tft.init();
    tft.setRotation(0); // 0 = Verticale (Portrait)
    tft.fillScreen(TFT_BLACK);
    
    tft.setTextColor(TFT_CYAN);
    tft.drawCentreString("Config Caricata!", 120, 160, 4); // Test con Font 4



    xTaskCreate(StatsUpdate_Task, "StatsUpdate_Task", 4096, NULL, 1, NULL);
    
    while(1) {
        ESP_LOGI("STATS", "Current_stats: Hunger: %d, Health: %d, Energy: %d, Happiness: %d, Evolution: %d", 
                stats.hungerLevel, stats.healthLevel, stats.energyLevel, stats.happinessLevel, currentState.evolution);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}