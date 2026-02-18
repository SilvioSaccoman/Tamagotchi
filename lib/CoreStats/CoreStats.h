/**
 * @file CoreStats.h
 * @brief This file defines the CoreStats available.
 */

#ifndef CORESTATS_H
#define CORESTATS_H

#include "Stats.h"
#include "Sprites.h"
#include "Activities.h"
#include "FreeRTOSConfig.h"
#include <freertos/portmacro.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_timer.h>

// Global pointer to the current animation, can be switched to change the displayed animation
extern Animation* currentAnimation;
extern bool isHatching; // Flag to indicate if the hatching process has started
extern bool isHatched; // Flag to indicate if the hatching process has completed
extern bool isMoving; // Flag to indicate if the Tamagotchi is currently moving
extern bool isEating; // Flag to indicate if the Tamagotchi is currently eating
extern bool isSleeping; // Flag to indicate if the Tamagotchi is currently sleeping
extern bool wasSleeping; // Flag to track if the Tamagotchi was sleeping in the previous cycle
extern bool wakingUp; // Flag to indicate if the Tamagotchi is in the process of

extern int childStartTime; // Variable to track the start time of the child stage

void StatsUpdate_Task(void* pvParameters);
void updateCurrentAnimation();

#endif // CORESTATS_H