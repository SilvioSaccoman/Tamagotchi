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

void pushImageFlipped(TFT_eSprite* sprite, int32_t x, int32_t y, int32_t w, int32_t h, const uint16_t* data) {
    for (int j = 0; j < h; j++) {
        for (int i = 0; i < w; i++) {
            // Legge il pixel normalmente, ma lo scrive da destra a sinistra (w - 1 - i)
            uint16_t pixel = pgm_read_word(data + (j * w + i));
            sprite->drawPixel(w - 1 - i, j, pixel);
        }
    }
}

void DisplayUpdate_Task(void* pvParameters) {
    int frameIdx = 0; // index

    // Of my egg, to be better handled once I have multiple animations
    const int totalFrames = 4; 
    
    int currentX = (tft.width() - SPRITE_EGG0_WIDTH) / 2;
    int yPos = tft.height() - SPRITE_EGG0_HEIGHT - 10;

    int targetX = currentX; // Target X position for walking
    int speed =2;
    bool facingLeft = false; // Direction the egg is facing

    while(1) {
        
        if (abs(currentX - targetX) < speed) {
            if (random(0, 100) < 5) { // 5% probability to change direction
                targetX = random(0, tft.width() - 64);
            }
        } else {
            // Move towards the target
            if (targetX > currentX) {
                currentX += speed;
                facingLeft = false; // Face right
            } else {
                currentX -= speed;
                facingLeft = true;  // Face left
            }
        }
        
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
        eggSprite.fillSprite(TFT_BLACK); 

        if (facingLeft) {
            pushImageFlipped(&eggSprite, 0, 0, 64, 64, animEGG[frameIdx]);
        } else {
            eggSprite.pushImage(0, 0, 64, 64, animEGG[frameIdx]);
        }

        eggSprite.pushSprite(currentX, yPos); // Push the sprite to the TFT at the specified position

        // Frame index update
        frameIdx = (frameIdx + 1) % totalFrames;

        vTaskDelay(pdMS_TO_TICKS(150)); 
    }
}


