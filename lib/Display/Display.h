/**
 * @file Display.h
 * @brief This file defines the Display functions available.
 */

#ifndef DISPLAY_H
#define DISPLAY_H

#include <TFT_eSPI.h>
#include "Sprites.h"
#include "CoreStats.h"

void Display_init();
void DisplayUpdate_Task(void* pvParameters);

#endif // DISPLAY_H