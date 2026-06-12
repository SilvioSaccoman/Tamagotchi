/**
 * @file CoreStats.h
 * @brief This file defines the CoreStats available.
 */

#ifndef CORESTATS_H
#define CORESTATS_H

#include "Stats.h"
#include "Sprites.h"
#include "FreeRTOSConfig.h"
#include <freertos/portmacro.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_timer.h>
#include "Memory.h"
#include "Gyro.h"
#include "Display.h"
#include "Microphone.h"

// Global pointer to the current animation, can be switched to change the displayed animation
extern Animation* currentAnimation;
extern bool isHatching; // Flag to indicate if the hatching process has started
extern bool isHatched; // Flag to indicate if the hatching process has completed
extern bool isMoving; // Flag to indicate if the Tamagotchi is currently moving
extern volatile bool isEating; // Flag to indicate if the Tamagotchi is currently eating
extern volatile bool isSleeping; // Flag to indicate if the Tamagotchi is currently sleeping
extern bool wasSleeping; // Flag to track if the Tamagotchi was sleeping in the previous cycle
extern volatile bool wakingUp; // Flag to indicate if the Tamagotchi is in the process of

extern int childStartTime; // Variable to track the start time of the child stage

// Timestamp of the last forced wake (ms since start). Used to avoid immediate re-sleep.
extern volatile uint32_t lastWakeMillis;

// Grace period (seconds) after a wake during which sensors won't force sleep
#define WAKE_GRACE_S 5

void StatsUpdate_Task(void* pvParameters);
void updateCurrentAnimation();
void Eating(struct Stats* stats);
void Sleeping(struct Stats* stats);

#endif // CORESTATS_H