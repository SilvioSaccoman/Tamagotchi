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
void pushImageFlipped(TFT_eSprite* sprite, int32_t x, int32_t y, int32_t w, int32_t h, const uint16_t* data);
void DisplayUpdate_Task(void* pvParameters);

#endif // DISPLAY_H