/**
 * @file Memory.h
 * @brief This file defines the Memory functions available.
 */

#ifndef MEMORY_H
#define MEMORY_H

#include <Preferences.h>
#include "Stats.h"

extern struct Stats stats;
extern struct State currentState;

void saveStats();
void loadStats();
void loadDefaultStats();

#endif // MEMORY_H