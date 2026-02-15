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
    int frameIdx = 0;
    int currentX = (tft.width() - 64) / 2;
    int yPos = tft.height() - 64 - 10;
    int targetX = currentX;
    int speed = 2;
    bool facingLeft = false;

    while(1) {
        
        updateCurrentAnimation(); // Update the current animation based on the current state

        // Safety check per cambio animazione
        if (frameIdx >= currentAnimation->frameCount) {
            frameIdx = 0;
        }

        // Movement logic: if the sprite is close to the target, choose a new random target with a small probability
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
        
        // Text update
        tft.setTextColor(TFT_WHITE, TFT_BLACK);
        tft.setCursor(0, 0);
        tft.setTextColor(TFT_WHITE);
        tft.setTextSize(2);
        tft.printf("Hunger: %d\n", stats.hungerLevel);
        tft.printf("Health: %d\n", stats.healthLevel);
        tft.printf("Energy: %d\n", stats.energyLevel);
        tft.printf("Happiness: %d\n", stats.happinessLevel);
        tft.printf("Evolution: %d\n", currentState.evolution);

        // Rendering Sprite
        TamagotchiSprite.fillSprite(TFT_BLACK); 
        const uint16_t* currentFrameData = currentAnimation->frames[frameIdx];

        if (facingLeft) {
            pushImageFlipped(&TamagotchiSprite, 0, 0, currentAnimation->width, currentAnimation->height, currentFrameData);
        } else {
            TamagotchiSprite.pushImage(0, 0, currentAnimation->width, currentAnimation->height, currentFrameData);
        }

        TamagotchiSprite.pushSprite(currentX, yPos);

        if (isHatching) {
            if (frameIdx >= currentAnimation->frameCount - 1) {
                isHatching = false; 
                currentState.evolution = CHILD;
                childStartTime = stats.life_seconds;
                frameIdx = 0;
            } else {
                frameIdx++;
            }
        } else {
            frameIdx = (frameIdx + 1) % currentAnimation->frameCount;
        }

        vTaskDelay(pdMS_TO_TICKS(200));
    }
}

