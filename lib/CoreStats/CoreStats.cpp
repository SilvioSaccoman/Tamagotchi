/**
 * @file CoreStats.c
 * @brief This file implements the Core logic behind the Tamagotchi's stats management.
 */

#include "CoreStats.h"

extern struct Stats stats;
extern struct State currentState;

bool isHatching = false;
bool isHatched = false;
bool isMoving = false;
bool wasSleeping = false; 
volatile bool wakingUp = false;

volatile uint32_t lastWakeMillis = 0;

Animation* currentAnimation = &eggAnimation;
int childStartTime = 0; // Variable to track the start time of the child stage

void StatsUpdate_Task(void* pvParameters) {

    TickType_t xLastWakeTime = xTaskGetTickCount();
    const TickType_t xFrequency = pdMS_TO_TICKS(1000);
    int64_t StartCycleTime = 0;

    int secondsCounter = 0;
    int lastEvolution = currentState.evolution;

    while (1) {

        // If the Tamagotchi is dead, we don't want to update its stats anymore.
        if (currentState.evolution == DEAD) {
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue; 
        }

        // Time update
        vTaskDelayUntil(&xLastWakeTime, xFrequency);
        uint32_t now = millis();
        stats.life_seconds++;
        secondsCounter++;
        StartCycleTime = esp_timer_get_time();
        ESP_LOGI("CoreStats", "Life seconds: %d", stats.life_seconds);
        ESP_LOGI("CoreStats", "Cycle time: %lld ms", StartCycleTime/1000);

        // Check if the display should be turned off due to inactivity
        update_display_timeout(now);

        // Update the Tamagotchi's stats and state based on the current conditions
        update_hunger(); // Hunger
        if (update_health()) { // Health (we skip the next updates if the Tamagotchi is dead)
            continue; 
        }
        update_energy();
        update_happiness();
        update_evolution();

        // Save the stats periodically and on evolution change
        handle_saving(&secondsCounter, &lastEvolution);
    }
}

void updateCurrentAnimation() {

    if (currentState.evolution == DEAD) {
        currentAnimation = &tombAnimation;
        return; // If dead, show tomb animation regardless of other states
    }

    // --------- Hatching animation ---------
    if (isHatching) {
        currentAnimation = &birthAnimation;
        return; // During hatching, we want to show the birth animation regardless of the state
    }

    // Check the current evolution stage and update the current animation accordingly
    // ---------- Egg animation ----------
    if (currentState.evolution == EGG) {
        currentAnimation = &eggAnimation;
    } 

    // ---------- Child animations ----------
    else if (currentState.evolution == CHILD) {
        // Sleeping
        if(isSleeping || wakingUp){
            currentAnimation = &childSleepAnimation;
            return;
        }
        //Eating
        if (isEating){
            currentAnimation = &childEatAnimation;
            return;
        }
        // Moving Animations
        if (isMoving) {    
            if (currentState.healthLevel == VERY_SICK) { // S
                currentAnimation = &childWalkAnimationS;
                if (currentState.hungerLevel == VERY_HUNGRY) { // HS
                    currentAnimation = &childWalkAnimationHS;
                }
            } else if (currentState.hungerLevel == VERY_HUNGRY) { // H
                currentAnimation = &childWalkAnimationH;
            } else {
                currentAnimation = &childWalkAnimation; // NONE
            }
        } else {
            // Idle Animations
            if (currentState.healthLevel == VERY_SICK) {
                currentAnimation = &childIdleAnimationS;
                if (currentState.hungerLevel == VERY_HUNGRY) {
                    currentAnimation = &childIdleAnimationHS;
                }
            } else if (currentState.hungerLevel == VERY_HUNGRY) {
                currentAnimation = &childIdleAnimationH;
            } else {
                currentAnimation = &childIdleAnimation;
            }
        }
    }

    // ------------ Teen animations ------------
    else if (currentState.evolution == TEENAGER) {
        //Sleeping
        if(isSleeping || wakingUp){
            currentAnimation = &teenSleepAnimation;
            return;
        }
        //Eating
        if (isEating){
            currentAnimation = &teenEatAnimation;
            return;
        }
        // Moving-Idle Animations    
        if (currentState.healthLevel == VERY_SICK) { // S
            currentAnimation = &teenWalkAnimationS;
            if (currentState.hungerLevel == VERY_HUNGRY) { // HS
                currentAnimation = &teenWalkAnimationHS;
            }
        } else if (currentState.hungerLevel == VERY_HUNGRY) { // H
            currentAnimation = &teenWalkAnimationH;
        } else {
            currentAnimation = &teenWalkAnimation; // NONE
        }
    }
    // ------------ Adult animations ------------
    else if (currentState.evolution == ADULT) {
        //Sleeping
        if(isSleeping || wakingUp){
            currentAnimation = &adultSleepAnimation;
            return;
        }
        //Eating
        if (isEating){
            currentAnimation = &adultEatAnimation;
            return;
        }
        // Moving-Idle Animations    
        if (currentState.healthLevel == VERY_SICK) { // S
            currentAnimation = &adultWalkAnimationS;
            if (currentState.hungerLevel == VERY_HUNGRY) { // HS
                currentAnimation = &adultWalkAnimationHS;
            }
        } else if (currentState.hungerLevel == VERY_HUNGRY) { // H
            currentAnimation = &adultWalkAnimationH;
        } else {
            currentAnimation = &adultWalkAnimation; // NONE
        }
    }
    // ------------ Elder animations ------------
    else if (currentState.evolution == ELDER) {
        //Sleeping
        if(isSleeping || wakingUp){
            currentAnimation = &elderSleepAnimation;
            return;
        }
        //Eating
        if (isEating){
            currentAnimation = &elderEatAnimation;
            return;
        }
        // Moving-Idle Animations    
        if (currentState.healthLevel == VERY_SICK) { // S
            currentAnimation = &elderWalkAnimationS;
            if (currentState.hungerLevel == VERY_HUNGRY) { // HS
                currentAnimation = &elderWalkAnimationHS;
            }
        } else if (currentState.hungerLevel == VERY_HUNGRY) { // H
            currentAnimation = &elderWalkAnimationH;
        } else {
            currentAnimation = &elderWalkAnimation; // NONE
        }
    }
}

static void update_display_timeout(uint32_t now) {
    if (isDisplayOn && (now - lastInteractionTime > DISPLAY_TIMEOUT)) {
        digitalWrite(TFT_BL, LOW); // Spegne la retroilluminazione (LED OFF)
        isDisplayOn = false;
        ESP_LOGI("CoreStats", "Display OFF per risparmio energetico");
    }
}

static void update_hunger(void) {
    // STATS UPDATE
    // Each 864 seconds it loses 1 point in hunger (50 points in 12 hours)
    if (stats.life_seconds % HUNGER_DECREASE_S == 0) {
        if (stats.hungerLevel > 0)
            stats.hungerLevel--;
        ESP_LOGI("CoreStats", "Hunger level: %d", stats.hungerLevel);
    }

    // STATE UPDATE 
    if (stats.hungerLevel >= 75) {
        currentState.hungerLevel = NOT_HUNGRY;
    } else if (stats.hungerLevel >= 50) {
        currentState.hungerLevel = SLIGHTLY_HUNGRY;
    } else if (stats.hungerLevel >= 25) {
        currentState.hungerLevel = HUNGRY;
    } else {
        currentState.hungerLevel = VERY_HUNGRY;
    }
}

static bool update_health(void) {
    // STATS UPDATE
    switch(currentState.hungerLevel){
        case NOT_HUNGRY:
            // Increase health by 1 point every hour when not hungry {
            if (stats.life_seconds % HEALTH_DE_INCREASE_S == 0){ 
                if (stats.healthLevel < 100){
                    stats.healthLevel++; 
                }
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
            if (stats.life_seconds % HEALTH_DE_INCREASE_S == 0) {
                stats.healthLevel--;
                ESP_LOGI("CoreStats", "Health level: %d", stats.healthLevel);
            }
            break;
    }

    // STATE UPDATE
    if (stats.healthLevel >= 75) {
        currentState.healthLevel = HEALTHY;
    } else if (stats.healthLevel >= 50) {
        currentState.healthLevel = SLIGHTLY_SICK;
    } else if (stats.healthLevel >= 25) {
        currentState.healthLevel = SICK;
    } else {
        currentState.evolution = DEAD;
        saveStats(); // Save the final state
        ESP_LOGI("CoreStats", "Your tamagotchi is dead...");
        return true; // Ritorna true se il Tamagotchi è morto
    }
    return false; // È ancora vivo
}

static void update_energy(void) {
    // ----------------------- NOISE & WAKE UP CHECK -----------------------
    if (noiseDetected) {
        if (isSleeping) {
            isSleeping = false;
            wakingUp = true;
            lastWakeMillis = millis();
        }
        noiseDetected = false;
    }
    
    // STATS UPDATE
    if (isSleeping){
        if (stats.energyLevel == 100){
            isSleeping = false; // Stop sleeping when energy is fully restored
            ESP_LOGI("CoreStats", "Energy fully restored, waking up...");
        }
        if (stats.energyLevel < 100 && stats.life_seconds % ENERGY_INCREASE_S == 0) { // Increase energy by 60 points every 8 hours
            stats.energyLevel++;
            ESP_LOGI("CoreStats", "Energy level: %d", stats.energyLevel);
        }
    } else {
         // Decrease energy by 80 points every 16 hours when not sleeping
        if (stats.life_seconds % ENERGY_DECREASE_S == 0 && stats.energyLevel > 0) {
            stats.energyLevel--;
            ESP_LOGI("CoreStats", "Energy level: %d", stats.energyLevel);
        }
    }

    // CONTROLLO SVENIMENTO
    if (stats.energyLevel == 0 && !isSleeping) {
        isSleeping = true; // Forziamo il sonno immediato
        wakingUp = false;
        stats.energyLevel = 1; 
        // Penalità sulla salute per lo svenimento
        if (stats.healthLevel > 20) {
            stats.healthLevel -= 20;
        } else {
            stats.healthLevel = 0; // Lo svenimento è stato fatale
        }
        ESP_LOGE("CoreStats", "Your Tamagotchi just fainted!");
    }
    
    // STATE UPDATE
    if (stats.energyLevel >= 75) {
        currentState.energyLevel = ENERGETIC;
    } else if (stats.energyLevel >= 50) {
        currentState.energyLevel = SLIGHTLY_TIRED;
    } else if (stats.energyLevel >= 25) {
        currentState.energyLevel = TIRED;
    } else {
        currentState.energyLevel = VERY_TIRED;
    }
}

static void update_happiness(void) {
    // STATS UPDATE
    if ((currentState.healthLevel != HEALTHY || currentState.hungerLevel != NOT_HUNGRY) && stats.life_seconds % HAPPINESS_DE_INCREASE_S == 0) {
        if (stats.happinessLevel > 0)
            stats.happinessLevel--;
        ESP_LOGI("CoreStats", "Happiness level: %d", stats.happinessLevel);
    } else {
        if (stats.life_seconds % HAPPINESS_DE_INCREASE_S == 0){
            if (stats.happinessLevel < 100)
                stats.happinessLevel++;
            ESP_LOGI("CoreStats", "Happiness level: %d", stats.happinessLevel);
        }
    }
    
    // STATE UPDATE
    if (stats.happinessLevel >= 75) {
        currentState.happinessLevel = HAPPY;
    } else if (stats.happinessLevel >= 50) {
        currentState.happinessLevel = SLIGHTLY_UNHAPPY;
    } else if (stats.happinessLevel >= 25) {
        currentState.happinessLevel = UNHAPPY;
    } else {
        currentState.happinessLevel = VERY_UNHAPPY;
    }
}

static void update_evolution(void) {
    if (stats.total_steps >= EggSteps && currentState.evolution == EGG && !isHatched) { 
        isHatching = true;
        isHatched = true;
        ESP_LOGI("CoreStats", "Evolution: Child");
    }
    else if (stats.total_steps - EggSteps >= ChildSteps && currentState.evolution == CHILD && stats.healthLevel > 75) {
        currentState.evolution = TEENAGER;
        ESP_LOGI("CoreStats", "Evolution: Teenager");
    }
    else if (stats.total_steps - (EggSteps + ChildSteps) >= TeenagerSteps && currentState.evolution == TEENAGER && stats.healthLevel > 75) {
        currentState.evolution = ADULT;
        ESP_LOGI("CoreStats", "Evolution: Adult");
    }
    else if (stats.total_steps - (EggSteps + ChildSteps + TeenagerSteps) >= AdultSteps && currentState.evolution == ADULT && stats.healthLevel > 75) {
        currentState.evolution = ELDER;
        ESP_LOGI("CoreStats", "Evolution: Elder");
    }
}

static void handle_saving(int* secondsCounter, int* lastEvolution) {
    if (*secondsCounter >= 900){ // Save every 15 minutes
        saveStats();
        *secondsCounter = 0;
    }

    if (currentState.evolution != *lastEvolution) {
        saveStats(); // Save immediately on evolution change
        *lastEvolution = currentState.evolution;
    }
}