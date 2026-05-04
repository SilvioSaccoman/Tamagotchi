#include "Gyro.h"

Adafruit_MPU6050 mpu;
float accX, accY, accZ;
int step_count = 0; // Inizializza il contatore di passi

void Gyroscope_Init() {
    // Inizializziamo il bus I2C sui tuoi pin specifici
    Wire.begin(I2C_SDA, I2C_SCL);

    if (!mpu.begin()) {
        Serial.println("ERRORE: MPU6050 non trovato!");
        return;
    }
    
    // Impostazioni standard
    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);
}

void Gyroscope_Task(void* pvParameters) {
    Gyroscope_Init();

    sensors_event_t a, g, temp;

    float acc_filtered = 9.81;   // gravità iniziale (m/s²)
    uint32_t last_step_time = 0;

    while (1) {
        mpu.getEvent(&a, &g, &temp);

        float ax = a.acceleration.x;
        float ay = a.acceleration.y;
        float az = a.acceleration.z;

        // Magnitudine accelerazione
        float acc_mag = sqrt(ax*ax + ay*ay + az*az);

        // Filtro passa-basso (stima gravità)
        acc_filtered = 0.9 * acc_filtered + 0.1 * acc_mag;

        // Componente dinamica
        float dynamic = acc_mag - acc_filtered;

        uint32_t now = millis();

        // Step detection
        if (dynamic > 1.2 && (now - last_step_time) > 300) {
            step_count++;
            last_step_time = now;

            ESP_LOGI("STEP", "Steps: %d", step_count);
        }

        vTaskDelay(pdMS_TO_TICKS(20)); // 50 Hz
    }
}