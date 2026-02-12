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
    ESP_LOGI("Stats", "Eating... Hunger level: %d", stats->hungerLevel);
}
