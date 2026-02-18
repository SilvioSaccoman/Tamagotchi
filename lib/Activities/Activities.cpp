#include "Activities.h"

// Function to update the stats based on the current state of the Tamagotchi
void Eating(struct Stats* stats){

    int hunger = stats->hungerLevel;

    if (hunger >= 75) {
        ESP_LOGI("Eating", "Not hungry");
        return; // Not hungry
    }
    hunger += 50;

    if(hunger > 100){
        stats->hungerLevel = 100;
    } else {
        stats->hungerLevel = hunger;
    }

    isEating = true; // Set the eating flag to true to trigger the eating animation
    isMoving = false; // Stop movement when eating

    ESP_LOGI("Stats", "Eating... Hunger level: %d", stats->hungerLevel);
}

void Sleeping(struct Stats* stats){

    int energy = stats->energyLevel;

    //if (energy >= 40) {
    //    ESP_LOGI("Sleeping", "Not tired");
    //    return; // Not tired
    //}
    
    if (isSleeping){
        ESP_LOGI("Sleeping", "Already sleeping");
        isSleeping = false; // Stop sleeping if already sleeping
        return; // Already sleeping
    } else {
        isSleeping = true; // Set the sleeping flag to true to trigger the sleeping animation
        isMoving = false; // Stop movement when sleeping
        stats->energyLevel = stats->energyLevel + 20; // Set energy to 0 when sleeping starts
    }

    ESP_LOGI("Stats", "Sleeping...");

}


void Birth(struct Stats* stats){
}


    
