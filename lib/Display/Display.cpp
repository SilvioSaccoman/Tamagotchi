/**
 * @file Display.cpp
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
    //tft.invertDisplay(true);
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
    int currentScale = 1;
    int currentX = tft.width() / 2; // Start centered
    int targetX = currentX;
    int lastX = currentX;
    bool facingLeft = false;
    
    // Track evolution to detect the exact moment of change
    int lastEvolution = currentState.evolution;

    // Fixed ground reference (20 pixels from the bottom)
    const int GROUND_Y = tft.height() - 20;

    TamagotchiSprite.deleteSprite(); 
    TamagotchiSprite.createSprite(192, 192); 

    int speed = 2;
    int moveProbability = 5; // 20% chance to start moving when idle

    currentX = (tft.width()-64) / 2;
    targetX = currentX;

    // Flag per gestire la pulizia iniziale della UI di morte una sola volta
    bool deathScreenInitialized = false;
    static int lastDays = -1;
    static int lastSteps = -1;
    while (1) {
        updateCurrentAnimation();

        // ===================================================================
        // DEATH SCREEN (GAME OVER LOGIC)
        // ===================================================================
        if (currentState.evolution == DEAD) {
            if (!deathScreenInitialized) {
                tft.fillScreen(TFT_BLACK);
                
                // --- TEXT COMPRESSED AND MOVED HIGHER ---
                tft.setTextColor(TFT_WHITE, TFT_BLACK);
                tft.setTextDatum(TC_DATUM); 
                
                tft.setTextSize(3);
                tft.drawString("R.I.P.", tft.width() / 2, 8); 
                
                // --- TIME CALCULATION WITH SAFE CASTING ---
                // Convertiamo esplicitamente in long per evitare conflitti con abs()
                long signed_seconds = (long)stats.life_seconds;
                uint32_t total_s = (signed_seconds < 0) ? -signed_seconds : signed_seconds;
                
                int months = total_s / (30 * 24 * 3600);
                total_s %= (30 * 24 * 3600);
                
                int days = total_s / (24 * 3600);
                total_s %= (24 * 3600);
                
                int hours = total_s / 3600;

                tft.setTextSize(2);
                tft.setCursor(10, 40); 
                tft.printf("Survived for:\n %02dm %02dd %02dh", months, days, hours);
                
                // Gestione sicura del segno anche per i passi
                long signed_steps = (long)stats.total_steps;
                long total_steps_abs = (signed_steps < 0) ? -signed_steps : signed_steps;

                tft.setCursor(10, 80); 
                tft.printf("Total Steps:\n %ld", total_steps_abs);

                deathScreenInitialized = true;
            }

            // --- ANIMATION LOOP FOR DEATH (2 FRAMES) ---
            currentScale = 3; 
            int displayDim = 64 * currentScale; // 192 pixel
            currentX = (tft.width() - displayDim) / 2;
            
            // --- SPRITE PUSHED TO THE ABSOLUTE BOTTOM ---
            int yPos = tft.height() - displayDim; 

            TamagotchiSprite.fillSprite(TFT_BLACK);
            if (currentAnimation->frameCount > 0) {
                if (frameIdx >= currentAnimation->frameCount) frameIdx = 0;
                
                const uint16_t* currentFrameData = currentAnimation->frames[frameIdx];
                drawScaledFrame(&TamagotchiSprite, currentFrameData, false, currentScale);
                
                frameIdx = (frameIdx + 1) % currentAnimation->frameCount;
            }
            TamagotchiSprite.pushSprite(currentX, yPos);

            vTaskDelay(pdMS_TO_TICKS(400));
            continue; 
        }
        // ===================================================================
        // STANDARD RENDERING LOGIC (ALIVE)
        // ===================================================================
        
        // --- DYNAMIC SCALE & Y OFFSET SELECTOR ---
        switch (currentState.evolution) {
            case EGG:
                currentScale = 1; 
                break;
            case CHILD:
                currentScale = 2;
                break;
            case TEENAGER:
                currentScale = 2;
                break;
            case ADULT:
                currentScale = 3;
                break;
            case ELDER:
                currentScale = 3;
                break;
            case DEAD:
                currentScale = 2;
                break;
        }
        int displayDim = 64 * currentScale; 

        // --- EVOLUTION CHANGE CLEANUP ---
        if (currentState.evolution != lastEvolution) {
            tft.fillRect(0, GROUND_Y - 140, tft.width(), 145, TFT_BLACK);
            currentX = (tft.width()-displayDim) / 2;
            targetX = currentX;
            lastEvolution = currentState.evolution;
        }

        // Vertical adjustment
        int yOffset = 0;
        if (currentState.evolution == EGG) {
            yOffset = 0;  
        } else if (currentState.evolution == CHILD) {
            yOffset = -50; 
        }

        int yPos = GROUND_Y - displayDim - yOffset;

        if (frameIdx >= currentAnimation->frameCount) frameIdx = 0;

        // --- MOVEMENT Variables ---
        switch (currentState.energyLevel) {
            case ENERGETIC:
                speed = 3;
                moveProbability = 20; 
                break;
            case SLIGHTLY_TIRED:
                speed = 2;
                moveProbability = 10; 
                break;
            case TIRED:
                speed = 1; 
                moveProbability = 5; 
                break;
            case VERY_TIRED:
                speed = 0; 
                moveProbability = 0; 
                break;
        }

        // --- MOVEMENT LOGIC ---
        if (currentState.evolution != EGG) { 
            lastX = currentX;
            if (!isHatching && !isEating && !isSleeping && !wakingUp && speed > 0) { 
                int distance = abs(currentX - targetX);
                
                if (distance < speed) {
                    if (isMoving) {
                        isMoving = false;
                        frameIdx = 0; 
                    }
                    if (random(0, 100) < moveProbability) {
                        targetX = random(0, tft.width() - displayDim);
                    }
                } else {
                    if (!isMoving) { 
                        isMoving = true; 
                        frameIdx = 0; 
                    }
                    if (targetX > currentX) {
                        currentX += speed; 
                        facingLeft = false; 
                    } else {
                        currentX -= speed; 
                        facingLeft = true; 
                    }
                }
            } else {
                if (isMoving) {
                    isMoving = false;
                    frameIdx = 0;
                }
            }
        }

        currentX = constrain(currentX, 0, tft.width() - displayDim);

        // --- ANTI-FLASH CLEANING ---
        if (currentX > lastX) {
            tft.fillRect(lastX, yPos, (currentX - lastX), displayDim, TFT_BLACK);
        } else if (currentX < lastX) {
            tft.fillRect(currentX + displayDim, yPos, (lastX - currentX), displayDim, TFT_BLACK);
        }

        // --- RENDERING ---
        TamagotchiSprite.fillSprite(TFT_BLACK); 
        const uint16_t* currentFrameData = currentAnimation->frames[frameIdx];
        drawScaledFrame(&TamagotchiSprite, currentFrameData, facingLeft, currentScale);

        // // UI Text Standard (English)
        // tft.setTextColor(TFT_WHITE, TFT_BLACK);
        // tft.setCursor(0, 0);
        // tft.setTextSize(2);
        // tft.printf("Hunger: %-3d\nHealth: %-3d\nSteps: %-1d\nMic: %.1f\naccTotal: %.2f", 
        //             stats.hungerLevel, stats.healthLevel, stats.total_steps, currentSoundLevel, accTotal);


        int currentDays = (int)(stats.life_seconds / 86400);
        int currentSteps = stats.total_steps;

        int barHeight = 44; // Alzata a 44px per ospitare comodamente il testo Size 2
        int boxWidth = 72;  // Allargata a 72px per contenere numeri grandi senza collisioni

        // ==========================================
        // 1. RIQUADRO SINISTRA: GIORNI (Calendario stretto + Numero Size 2)
        // ==========================================
        if (currentDays != lastDays) {
            int leftX = 6; // Margine dal bordo sinistro
            
            // Cancella l'area del riquadro
            tft.fillRect(leftX, 0, boxWidth, barHeight, TFT_BLACK);
            
            // Icona Calendario Più Stretta (Larghezza: 15px, Altezza: 14px)
            int calX = leftX + (boxWidth - 15) / 2; // Centratura orizzontale
            int calY = 4;
            tft.drawRect(calX, calY + 3, 15, 11, TFT_WHITE); // Corpo stretto
            tft.fillRect(calX + 2, calY, 2, 4, TFT_WHITE);  // Anello sx
            tft.fillRect(calX + 11, calY, 2, 4, TFT_WHITE); // Anello dx
            tft.drawPixel(calX + 5, calY + 7, TFT_WHITE);   // Puntino interno 1
            tft.drawPixel(calX + 9, calY + 10, TFT_WHITE);  // Puntino interno 2
            
            // Configura testo BIG
            tft.setTextSize(2); // <--- Grandezza raddoppiata!
            tft.setTextColor(TFT_WHITE, TFT_BLACK);
            
            // Calcola larghezza (A Size 2, ogni carattere occupa 12 pixel in larghezza)
            char daysStr[8];
            sprintf(daysStr, "%d", currentDays);
            int textWidth = strlen(daysStr) * 12;
            
            // Stampa il numero centrato sotto il calendario
            tft.setCursor(leftX + (boxWidth - textWidth) / 2, 24);
            tft.print(daysStr);
            
            lastDays = currentDays;
        }

        // ==========================================
        // 2. RIQUADRO DESTRA: PASSI (Zampina + Numero Size 2)
        // ==========================================
        if (currentSteps != lastSteps) {
            int rightX = tft.width() - boxWidth - 6; // Bordo destro
            
            // Cancella l'area del riquadro
            tft.fillRect(rightX, 0, boxWidth, barHeight, TFT_BLACK);
            
            // Icona Zampina di Animale (Larghezza: 14px, Altezza: 11px)
            int pawX = rightX + (boxWidth - 14) / 2; // Centrata nel riquadro
            int pawY = 4;
            
            // I 4 polpastrelli superiori (piccoli blocchi 2x2)
            tft.fillRect(pawX,       pawY + 3, 2, 2, TFT_WHITE); // Dito esterno sx
            tft.fillRect(pawX + 3,   pawY,     2, 2, TFT_WHITE); // Dito interno sx
            tft.fillRect(pawX + 9,   pawY,     2, 2, TFT_WHITE); // Dito interno dx
            tft.fillRect(pawX + 12,  pawY + 3, 2, 2, TFT_WHITE); // Dito esterno dx
            
            // Il grande cuscinetto centrale inferiore (metacarpale)
            tft.fillRect(pawX + 3,   pawY + 6, 8, 5, TFT_WHITE); // Blocco principale
            tft.drawPixel(pawX + 2,  pawY + 8, TFT_WHITE);       // Arrotondamento sx
            tft.drawPixel(pawX + 11, pawY + 8, TFT_WHITE);       // Arrotondamento dx
            
            // Configura testo BIG
            tft.setTextSize(2); // <--- Grandezza raddoppiata!
            tft.setTextColor(TFT_WHITE, TFT_BLACK);
            
            // Calcola larghezza dinamica dei passi (caratteri * 12 pixel)
            char stepsStr[12];
            sprintf(stepsStr, "%d", currentSteps);
            int textWidth = strlen(stepsStr) * 12;
            
            // Stampa il passo centrato sotto la zampina
            tft.setCursor(rightX + (boxWidth - textWidth) / 2, 24);
            tft.print(stepsStr);
            
            lastSteps = currentSteps;
        }

        // ==========================================
        // 3. LINEA DI CHIUSURA BARRA (Abbassata a Y=44)
        // ==========================================
        if (currentDays != lastDays || currentSteps != lastSteps) {
            tft.drawFastHLine(0, 44, tft.width(), TFT_WHITE);
        }

        // Push to screen
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
        else if (isSleeping || wakingUp) {

    // CASO 1: Il Tamagotchi si sta svegliando (forzato o naturale)
    if (wakingUp) {
        if (frameIdx > 0) {
            frameIdx--;
        } 
    }

    // CASO 2: Sta dormendo normalmente
    else {

        // Rileva il primo istante in cui si addormenta per partire dal frame 0
        if (!wasSleeping) {
            frameIdx = 0;
            wasSleeping = true;
        }

        // Avanzamento normale del sonno
        if (frameIdx < 2) {
            frameIdx++;
        } else {
            frameIdx = (frameIdx == 2) ? 3 : 2;
        }
    }
}
        else {
            // Animazione standard (Idle / Walk) quando è sveglio
            wasSleeping = false; 
            
            // PROTEZIONE ANTI-BUG: Se cambiamo animazione e il vecchio frameIdx è fuori dai limiti della nuova, resettiamo a 0
            if (currentAnimation->frameCount > 0) {
                if (frameIdx >= currentAnimation->frameCount) {
                    frameIdx = 0;
                }
                frameIdx = (frameIdx + 1) % currentAnimation->frameCount;
            } else {
                frameIdx = 0;
            }
        }

        vTaskDelay(pdMS_TO_TICKS(300));
    }
}