/**
 * @file CoreStats.c
 * @brief This file implements the Core logic behind the Tamagotchi's stats management.
 */

#include "CoreStats.h"

extern struct Stats stats;
extern struct State currentState;

bool isHatching = false;
bool isHatched = false;
Animation* currentAnimation = &eggAnimation;
int childStartTime = 0; // Variable to track the start time of the child stage


void StatsUpdate_Task(void* pvParameters) {
    // Variables to track the start time of each evolution stage
    int teenagerStartTime = 0; // Variable to track the start time of the teenager
    int adultStartTime = 0; // Variable to track the start time of the adult stage

    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(1000);
    int64_t StartCycleTime = 0;

    while (1) {
        // ---------------------------- TIME UPDATE ---------------------------
        // We wait for the cycle to be 1 second long
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
        stats.life_seconds++;
        StartCycleTime = esp_timer_get_time();
        ESP_LOGI("CoreStats", "Life seconds: %d", stats.life_seconds);
        ESP_LOGI("CoreStats", "Cycle time: %lld ms", StartCycleTime/1000);

        // --------------------------- HUNGER UPDATE ---------------------------
        // Each 864 seconds it loses 1 point in hunger (50 points in 12 hours)
        if (stats.life_seconds % 864 == 0) {
            stats.hungerLevel--;
            ESP_LOGI("CoreStats", "Hunger level: %d", stats.hungerLevel);
        }

        // --------------------------- HEALTH UPDATE ---------------------------
        switch(stats.hungerLevel){
            case NOT_HUNGRY:
            // Increase health by 1 point every hour when not hungry {
            if (stats.healthLevel % 3600 == 0){ 
                stats.healthLevel++; 
            }
                break;
            case SLIGHTLY_HUNGRY:
                // Decrease health by 1 point every 12 hours
                if (stats.life_seconds % (DAY_DURATION / 2) == 0) {
                    stats.healthLevel--;
                    ESP_LOGI("CoreStats", "Health level: %d", stats.healthLevel);
                }
                break;
            case HUNGRY:
                // Decrease health by 1 point every 6 hours
                if (stats.life_seconds % (DAY_DURATION / 4) == 0) {
                    stats.healthLevel--;
                    ESP_LOGI("CoreStats", "Health level: %d", stats.healthLevel);
                }
                break;
            case VERY_HUNGRY:
                // Decrease health by 1 point every hour
                if (stats.life_seconds % 3600 == 0) {
                    stats.healthLevel--;
                    ESP_LOGI("CoreStats", "Health level: %d", stats.healthLevel);
                }
                break;
        }

        // --------------------------- ENERGY UPDATE ---------------------------
        if (stats.life_seconds % 900 == 0 && stats.energyLevel < 100) { // Increase energy by 4 point every hour
            stats.energyLevel++;
            ESP_LOGI("CoreStats", "Energy level: %d", stats.energyLevel);
        }


        // --------------------------- HAPPINESS UPDATE ---------------------------


        // --------------------------- EVOLUTION UPDATE ---------------------------
        if (stats.life_seconds >= EggDuration && currentState.evolution == EGG && stats.healthLevel > 75 && !isHatched) {
            isHatching = true;
            isHatched = true;
            ESP_LOGI("CoreStats", "Evolution: Child");
        }
        else if (stats.life_seconds - childStartTime >= ChildDuration && currentState.evolution == CHILD && stats.healthLevel > 75) {
            currentState.evolution = TEENAGER;
            teenagerStartTime = stats.life_seconds;
            ESP_LOGI("CoreStats", "Evolution: Teenager");
        }
        else if (stats.life_seconds - teenagerStartTime >= TeenagerDuration && currentState.evolution == TEENAGER && stats.healthLevel > 75) {
            currentState.evolution = ADULT;
            adultStartTime = stats.life_seconds;
            ESP_LOGI("CoreStats", "Evolution: Adult");
        }
        else if (stats.life_seconds - adultStartTime >= AdultDuration && currentState.evolution == ADULT && stats.healthLevel > 75) {
            currentState.evolution = ELDER;
            ESP_LOGI("CoreStats", "Evolution: Elder");
        }
    }
}

void updateCurrentAnimation() {
    if (isHatching) {
        currentAnimation = &birthAnimation;
        return; // During hatching, we want to show the birth animation regardless of the state
    }
    // Check the current evolution stage and update the current animation accordingly
    if (currentState.evolution == EGG) {
        currentAnimation = &eggAnimation;
    } 
    else if (currentState.evolution == CHILD) {
        // 2. Controllo Stati prioritari (Malattia > Fame > Camminata)
        if (stats.healthLevel < 20) {
            currentAnimation = &childWalkAnimationS;
            if (stats.hungerLevel < 40) {
                currentAnimation = &childWalkAnimationHS;
            }
        } else if (stats.hungerLevel < 40) {
            currentAnimation = &childWalkAnimationH;
        } else {
            currentAnimation = &childWalkAnimation;
        }
    }
}