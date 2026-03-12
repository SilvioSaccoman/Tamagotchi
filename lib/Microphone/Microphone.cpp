#include "Microphone.h"

volatile float currentSoundLevel = 0; // Changed by the task

void Microphone_Init() {
    const i2s_config_t i2s_config = {
        .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
        .sample_rate = 16000,
        .bits_per_sample = I2S_BITS_PER_SAMPLE_32BIT,
        .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
        .communication_format = I2S_COMM_FORMAT_STAND_I2S,
        .intr_alloc_flags = ESP_INTR_FLAG_LEVEL1,
        .dma_buf_count = 8,
        .dma_buf_len = 64,
        .use_apll = false
    };

    const i2s_pin_config_t pin_config = {
        .bck_io_num = I2S_SCK,
        .ws_io_num = I2S_WS,
        .data_out_num = -1, // Non usiamo l'uscita
        .data_in_num = I2S_SD
    };

    i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
    i2s_set_pin(I2S_PORT, &pin_config);
}

void Microphone_Task(void* pvParameters) {
    Microphone_Init(); // Initialization
    
    int32_t samples[128]; // Temporary buffer for I2S data
    size_t bytes_read;

    while (1) {
        // Read audio data from I2S
        i2s_read(I2S_PORT, &samples, sizeof(samples), &bytes_read, portMAX_DELAY);

        if (bytes_read > 0) {
            int num_samples = bytes_read / sizeof(int32_t);
            float sumSquared = 0;

            for (int i = 0; i < num_samples; i++) {
                // Converts the raw sample to a float value. The shift is based on the assumption of 24-bit data in a 32-bit container.
                float sampleVal = (float)(samples[i] >> 14); 
                sumSquared += sampleVal * sampleVal;
            }

            // Calculate the RMS (Root Mean Square), which is much more stable than the simple average
            float rms = sqrt(sumSquared / num_samples);
            
            // Apply a slight "smoothing" to prevent the value from jumping too quickly
            currentSoundLevel = (currentSoundLevel * 0.7) + (rms * 0.3);
        }

        // Small delay
        vTaskDelay(pdMS_TO_TICKS(10)); 
    }
}