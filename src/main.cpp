#include <Arduino.h>
#include "Display.h"
#include "CoreStats.h"
#include "Sprites.h"

TFT_eSPI tft = TFT_eSPI();

// Initialization of the stats and state
struct Stats stats = {
    .hungerLevel = 20,
    .healthLevel = 100,
    .energyLevel = 20,
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

#define BUTTON_RESET_PIN 0 // Often the 'BOOT' button on ESP32 boards

// Dedicated Input Task
void Input_Task(void* pvParameters) {
    pinMode(BUTTON_RESET_PIN, INPUT_PULLUP);
    bool lastState = HIGH;

    while (1) {
        bool buttonState = digitalRead(BUTTON_RESET_PIN); 
        
        // Simple Debounce: Trigger on Falling Edge (Pressed)
        if (lastState == HIGH && buttonState == LOW) {
            if (currentState.evolution != EGG) {
                Sleeping(&stats);
            }
            vTaskDelay(pdMS_TO_TICKS(200)); // Debounce delay
        }
        
        lastState = buttonState;
        vTaskDelay(pdMS_TO_TICKS(50)); // Poll every 50ms
    }
}

extern "C" void app_main() {
    initArduino();
    Display_init();    
    
    xTaskCreate(Input_Task, "Input_Task", 4096, NULL, 1, NULL);
    xTaskCreate(StatsUpdate_Task, "StatsUpdate_Task", 4096, NULL, 1, NULL);
    xTaskCreatePinnedToCore(DisplayUpdate_Task, "DisplayUpdate_Task", 4096, NULL, 1, NULL, 1); // Run the display task on core 1 to avoid conflicts with the stats update task
    
    while(1) {
        ESP_LOGI("STATS", "Current_stats: Hunger: %d, Health: %d, Energy: %d, Happiness: %d, Evolution: %d", 
                stats.hungerLevel, stats.healthLevel, stats.energyLevel, stats.happinessLevel, currentState.evolution);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}