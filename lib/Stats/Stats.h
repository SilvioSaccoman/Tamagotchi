/**
 * @file Stats.h
 * @brief This file defines the Stats struct and related enums for the Tamagotchi project.
 */

#ifndef STATS_H
#define STATS_H

#include <esp_log.h>

#define DAY_DURATION 86400 // Duration of a day in seconds

#define EggDuration DAY_DURATION / 86400 * 10        // Duration of the egg stage in seconds      [1 hour]
#define ChildDuration DAY_DURATION /86400 * 10        // Duration of the child stage in seconds    [8 days]
#define TeenagerDuration DAY_DURATION /86400 * 10      // Duration of the teenager stage in seconds [8 days]
#define AdultDuration DAY_DURATION * 38        // Duration of the adult stage in seconds    [38 days]
#define ElderDuration DAY_DURATION * 38        // Duration of the elder stage in seconds    [38 days]

#define HEALTH_DE_INCREASE_S 3600 // Increase health by 1 point every hour when not hungry
#define HUNGER_DECREASE_S 864 // Decrease hunger by 1 point every 864 seconds (50 points in 12 hours)
#define ENERGY_INCREASE_S 480 // Increase energy by 60 points every 480 seconds (8 hours)
#define ENERGY_DECREASE_S 720 // Decrease energy by 80 points every 720 seconds (16 hours)

// Tamagotch evolution stages
enum Evolution {
    EGG,
    CHILD,
    TEENAGER,
    ADULT,
    ELDER
};

// Tamagotchi hungy levels
enum HungerLevel {
    NOT_HUNGRY,                // 100-75
    SLIGHTLY_HUNGRY,           // 74-50
    HUNGRY,                    // 49-25
    VERY_HUNGRY                // 24-0     
};

// Tamagotchi health levels
enum HealthLevel {
    HEALTHY,                    // 100-75
    SLIGHTLY_SICK,              // 74-50
    SICK,                       // 49-25
    VERY_SICK                   // 24-0     
};

// Tamagotchi energy levels
enum EnergyLevel {
    ENERGETIC,                  // 100-75
    SLIGHTLY_TIRED,             // 74-50
    TIRED,                      // 49-25
    VERY_TIRED                   // 24-0     
};

// Tamagotchi happiness levels
enum HappinessLevel {
    HAPPY,                      // 100-75
    SLIGHTLY_UNHAPPY,           // 74-50
    UNHAPPY,                    // 49-25
    VERY_UNHAPPY                // 24-0     
};

// Current stats of the Tamagotchi
struct Stats
{
    int hungerLevel;             // 0-100
    int healthLevel;             // 0-100
    int energyLevel;             // 0-100
    int happinessLevel;          // 0-100
    int life_seconds;
};

// Current state of the Tamagotchi
struct State{
    enum Evolution evolution;
    enum HungerLevel hungerLevel;
    enum HealthLevel healthLevel;
    enum EnergyLevel energyLevel;
    enum HappinessLevel happinessLevel;
};

#endif // STATS_H