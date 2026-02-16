/**
 * @file CoreStats.c
 * @brief This file implements the Task that handles  logic behind the Tamagotchi's stats management.
 */

#include "Display.h"

extern TFT_eSPI tft;
extern struct Stats stats;
extern struct State currentState;

TFT_eSprite TamagotchiSprite = TFT_eSprite(&tft); // Create the sprite

void Display_init() {
    tft.init();
    tft.setRotation(0); // 0 = Vertical (Portrait), 1 = Horizontal (Landscape), 2 = Vertical Inverted, 3 = Horizontal Inverted
    tft.fillScreen(TFT_BLACK);
    TamagotchiSprite.createSprite(64, 64); // Allocate 32x32 pixel in RAM 
};

/**
 * Draws a 64x64 frame into the sprite with optional flipping and custom scaling.
 * @param sprite Pointer to the TFT_eSprite
 * @param data Pointer to the 64x64 pixel array in PROGMEM
 * @param flip Boolean to flip horizontally
 * @param scale The multiplier (1, 2, etc.)
 */
void drawScaledFrame(TFT_eSprite* sprite, const uint16_t* data, bool flip, int scale) {
    const int srcSize = 64;
    
    for (int j = 0; j < srcSize; j++) {
        for (int i = 0; i < srcSize; i++) {
            uint16_t pixel = pgm_read_word(data + (j * srcSize + i));
            
            // Calculate coordinates based on flip
            int xIdx = flip ? (srcSize - 1 - i) : i;
            
            // Scale the single pixel into a square block
            if (scale == 1) {
                sprite->drawPixel(xIdx, j, pixel);
            } else {
                sprite->fillRect(xIdx * scale, j * scale, scale, scale, pixel);
            }
        }
    }
}

void DisplayUpdate_Task(void* pvParameters) {
    int frameIdx = 0;
    int currentX = tft.width() / 2;
    int targetX = currentX;
    int lastX = currentX;
    bool facingLeft = false;
    
    // Track evolution to detect the exact moment of change
    int lastEvolution = currentState.evolution;

    // Fixed ground reference (20 pixels from the bottom)
    const int GROUND_Y = tft.height() - 20;

    TamagotchiSprite.deleteSprite(); 
    TamagotchiSprite.createSprite(128, 128); 

    while (1) {
        updateCurrentAnimation();

        // --- DYNAMIC SCALE & Y OFFSET SELECTOR ---
        int currentScale = (currentState.evolution == EGG) ? 1 : 2; 
        int displayDim = 64 * currentScale; 

        // Vertical adjustment
        int yOffset = 0;
        if (currentState.evolution == EGG) {
            yOffset = 0;  // Egg is fine at ground level
        } else if (currentState.evolution == CHILD) {
            yOffset = -50; // 
        }

        // Final Y position based on ground, sprite size, and specific offset
        int yPos = GROUND_Y - displayDim - yOffset;

        if (frameIdx >= currentAnimation->frameCount) frameIdx = 0;

        // --- EVOLUTION CHANGE CLEANUP ---
        // If we just evolved, clear the whole bottom area once to remove old sprite remnants
        if (currentState.evolution != lastEvolution) {
            tft.fillRect(0, GROUND_Y - 140, tft.width(), 145, TFT_BLACK);
            lastEvolution = currentState.evolution;
        }

        // --- MOVEMENT LOGIC ---
        lastX = currentX;
        if (!isHatching) {
            if (abs(currentX - targetX) < 2) {
                isMoving = false;
                if (random(0, 100) < 5) {
                    targetX = random(0, tft.width() - displayDim);
                }
            } else {
                isMoving = true;
                if (targetX > currentX) {
                    currentX += 2; facingLeft = false; 
                }
                else { currentX -= 2; facingLeft = true; }
            }
        }

        currentX = constrain(currentX, 0, tft.width() - displayDim);

        // --- ANTI-FLASH CLEANING (Standard Movement) ---
        if (currentX > lastX) {
            tft.fillRect(lastX, yPos, (currentX - lastX), displayDim, TFT_BLACK);
        } else if (currentX < lastX) {
            tft.fillRect(currentX + displayDim, yPos, (lastX - currentX), displayDim, TFT_BLACK);
        }

        // --- RENDERING ---
        TamagotchiSprite.fillSprite(TFT_BLACK); 
        const uint16_t* currentFrameData = currentAnimation->frames[frameIdx];

        drawScaledFrame(&TamagotchiSprite, currentFrameData, facingLeft, currentScale);

        // UI Text
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.setCursor(0, 0);
        tft.setTextSize(2);
        tft.printf("Hunger: %-3d\nHealth: %-3d\nEvolution: %-1d\n", 
                   stats.hungerLevel, stats.healthLevel, currentState.evolution);

        // Push to screen at the calculated yPos
        TamagotchiSprite.pushSprite(currentX, yPos);

        // --- FRAME ADVANCEMENT ---
        if (isHatching) {
            if (frameIdx >= (currentAnimation->frameCount - 1)) {
                isHatching = false; 
                currentState.evolution = CHILD;
                frameIdx = 0; 
            } else {
                frameIdx++;
            }
        } 
        else if (isEating) {
            if (frameIdx >= (currentAnimation->frameCount - 1)) {
                isEating = false; 
                frameIdx = 0; 
            } else {
                frameIdx++;
            }
        } 
        else {
            if (currentAnimation->frameCount > 0) {
                frameIdx = (frameIdx + 1) % currentAnimation->frameCount;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(200));
    }
}