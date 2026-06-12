#include "Microphone.h"
#include "driver/gpio.h" // Aggiungi questo per il controllo diretto

volatile float currentSoundLevel = 0;
volatile bool noiseDetected = false;

void Microphone_Init() {
    // Forza la configurazione del pin per evitare il messaggio "InputEn: 0"
    gpio_config_t io_conf = {};
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = (1ULL << MIC_PIN);
    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    io_conf.pull_up_en = GPIO_PULLUP_DISABLE;
    gpio_config(&io_conf);

    Serial.println("Microphone Initialized.");
}

void Microphone_Task(void* pvParameters) {
    Microphone_Init();
    // Riduciamo la frequenza di campionamento per non intasare i log
    while (1) {
        int min_v = 4095;
        int max_v = 0;

        // Campionamento Peak-to-Peak per 20ms
        uint32_t start = millis();
        while (millis() - start < 20) {
            int val = analogRead(MIC_PIN);
            if (val > max_v) max_v = val;
            if (val < min_v) min_v = val;
        }

        int diff = max_v - min_v;
        float level = (diff * 100.0) / 4095.0;

        // Filtro passa-basso per stabilità
        currentSoundLevel = (currentSoundLevel * 0.9) + (level * 0.1);

        // Stampa solo se il livello è significativo per evitare spam
        if (currentSoundLevel > 50.0) {
             printf("Mic Level: %.2f\n", currentSoundLevel);
             noiseDetected = true;

        }

        vTaskDelay(pdMS_TO_TICKS(50)); 
    }
}