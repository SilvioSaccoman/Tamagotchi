/**
 * @file Sprites.h
 * @brief This file defines all the possible Sprites available.
 */

#ifndef SPRITES_H
#define SPRITES_H

#include <pgmspace.h>
#include <stdint.h>

// -------------- EGG STATE SPRITES --------------
#define SPRITE_EGG0_HEIGHT 64
#define SPRITE_EGG0_WIDTH 64

extern const uint16_t sprite_EGG0[] PROGMEM;
extern const uint16_t sprite_EGG1[] PROGMEM;
extern const uint16_t sprite_EGG2[] PROGMEM;

extern const uint16_t* const animEGG[] PROGMEM;






#endif // SPRITES_H