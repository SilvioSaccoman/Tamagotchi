#include "Memory.h"

Preferences prefs;

void saveStats() {
    prefs.begin("tamagotchi", false); // Open in write mode
    
    // STATS
    prefs.putInt("hunger", stats.hungerLevel);
    prefs.putInt("health", stats.healthLevel);
    prefs.putInt("energy", currentState.energyLevel);
    prefs.putInt("happiness", currentState.happinessLevel);
    prefs.putInt("life_seconds", stats.life_seconds);

    // STATE
    prefs.putInt("evol", currentState.evolution);

    prefs.end(); // Chiude la memoria
    ESP_LOGI("Memory", "Stats Saved!");
}

void loadStats() {
    prefs.begin("tamagotchi", true); // Open in reading mode
    
    // Reads the stats, if they don't exist it will return the default value
    // STATS
    stats.hungerLevel = prefs.getInt("hunger", 100);
    stats.healthLevel = prefs.getInt("health", 100);
    stats.happinessLevel = prefs.getInt("happiness", 100);
    stats.energyLevel = prefs.getInt("energy", 100);
    stats.life_seconds = prefs.getInt("life_seconds", 0);

    //STATE
    currentState.evolution = (Evolution)prefs.getInt("evol", EGG);
    
    prefs.end();
    ESP_LOGI("Memory", "Stats Loaded!");
}

void loadDefaultStats() {
    // STATS
    stats.hungerLevel = 100;
    stats.healthLevel = 100;
    stats.energyLevel = 100;
    stats.happinessLevel = 100;
    stats.life_seconds = 0;

    // STATE
    currentState.evolution = EGG;

    saveStats(); // Save the default stats to memory
}