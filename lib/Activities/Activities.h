/**
 * @file Activities.h
 * @brief This file defines the Activities available.
 */

#ifndef ACTIVITIES_H
#define ACTIVITIES_H

#include "Stats.h"

extern bool isEating; // Flag to indicate if the Tamagotchi is currently eating
extern bool isSleeping; // Flag to indicate if the Tamagotchi is currently sleeping
extern bool isMoving; // Flag to indicate if the Tamagotchi is currently moving

void Eating(struct Stats* stats);
void Sleeping(struct Stats* stats);

#endif // ACTIVITIES_H