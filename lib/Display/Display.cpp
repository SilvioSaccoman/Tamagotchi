/**
 * @file CoreStats.c
 * @brief This file implements the Task that handles  logic behind the Tamagotchi's stats management.
 */

#include "Display.h"

extern TFT_eSPI tft;
extern struct Stats stats;
extern struct State currentState;

TFT_eSprite eggSprite = TFT_eSprite(&tft); // Create the sprite

void Display_init() {
    tft.init();
    tft.setRotation(0); // 0 = Vertical (Portrait), 1 = Horizontal (Landscape), 2 = Vertical Inverted, 3 = Horizontal Inverted
    tft.fillScreen(TFT_BLACK);
    eggSprite.createSprite(64, 64); // Allocate 32x32 pixel in RAM 
};

void DisplayUpdate_Task(void* pvParameters) {
    int frameIdx = 0; // index

    // Of my egg, to be better handled once I have multiple animations
    const int totalFrames = 4; 
    
    int xPos = (tft.width() - SPRITE_EGG0_WIDTH) / 2;
    int yPos = tft.height() - SPRITE_EGG0_HEIGHT - 10;

    while(1) {
        
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        
        // Stats update
        tft.setCursor(0, 0);
        tft.setTextColor(TFT_WHITE);
        tft.setTextSize(2);
        tft.printf("Hunger: %d\n", stats.hungerLevel);
        tft.printf("Health: %d\n", stats.healthLevel);
        tft.printf("Energy: %d\n", stats.energyLevel);
        tft.printf("Happiness: %d\n", stats.happinessLevel);
        tft.printf("Evolution: %d\n", currentState.evolution);

        // Frame update
        eggSprite.pushImage(0, 0, 64, 64, animEGG[frameIdx]);
        eggSprite.pushSprite(xPos, yPos); // Push the sprite to the TFT at the specified position

        // Frame index update
        frameIdx = (frameIdx + 1) % totalFrames;

        vTaskDelay(pdMS_TO_TICKS(300)); 
    }
}


