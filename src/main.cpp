#include <Arduino.h>
#include <TFT_eSPI.h>

TFT_eSPI tft = TFT_eSPI();

extern "C" void app_main() {
    initArduino();
    
    tft.init();
    tft.setRotation(0); // 0 = Verticale (Portrait)
    tft.fillScreen(TFT_BLACK);
    
    tft.setTextColor(TFT_CYAN);
    tft.drawCentreString("Config Caricata!", 120, 160, 4); // Test con Font 4
    
    while(1) {
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}