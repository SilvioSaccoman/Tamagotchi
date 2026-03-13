#include "Gyro.h"

Adafruit_MPU6050 mpu;
float gyroX, gyroY, gyroZ;

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
    while (1) {
        mpu.getEvent(&a, &g, &temp);

        // Salviamo i valori dell'accelerometro
        gyroX = a.acceleration.x;
        gyroY = a.acceleration.y;
        gyroZ = a.acceleration.z;

        ESP_LOGI("GYRO", "Gyro X: %.2f, Gyro Y: %.2f, Gyro Z: %.2f", gyroX, gyroY, gyroZ);

        vTaskDelay(pdMS_TO_TICKS(100)); // Leggiamo 10 volte al secondo
    }
}