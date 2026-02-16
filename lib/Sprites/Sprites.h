/**
 * @file Sprites.h
 * @brief This file defines all the possible Sprites available.
 */

#ifndef SPRITES_H
#define SPRITES_H

#include <pgmspace.h>
#include <stdint.h>

struct Animation {
    const uint16_t* const* frames; // Array of pointers to frame data
    int width;               // Width of each frame
    int height;              // Height of each frame
    uint8_t frameCount;             // Number of frames in the animation
};

// -------------- EGG STATE SPRITES --------------

extern const uint16_t sprite_EGG0[] PROGMEM;
extern const uint16_t sprite_EGG1[] PROGMEM;
extern const uint16_t sprite_EGG2[] PROGMEM;

extern const uint16_t* const animEGG[] PROGMEM;

// -------------- ANIMATIONS --------------
// EGG
extern Animation eggAnimation;

// HATCHING
extern Animation birthAnimation;

// CHILD
extern Animation childWalkAnimation; // Walking
extern Animation childWalkAnimationH;
extern Animation childWalkAnimationS;
extern Animation childWalkAnimationHS;

extern Animation childIdleAnimation; // Idle
extern Animation childIdleAnimationH;
extern Animation childIdleAnimationS;
extern Animation childIdleAnimationHS;

extern Animation childEatAnimation; // Eating
extern Animation childEatAnimationH;
extern Animation childEatAnimationS;
extern Animation childEatAnimationHS;

extern Animation childSleepAnimation; // Sleeping

#endif // SPRITES_H