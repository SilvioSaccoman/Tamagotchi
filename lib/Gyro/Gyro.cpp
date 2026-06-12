#include "Gyro.h"

Adafruit_MPU6050 mpu;
float accX, accY, accZ;
Madgwick filter;

bool isDisplayOn = true;
uint32_t lastInteractionTime = 0;
const uint32_t DISPLAY_TIMEOUT = 30000;

extern struct Stats stats;

void Gyroscope_Init() {
    Wire.begin(I2C_SDA, I2C_SCL);

    if (!mpu.begin()) {
        Serial.println("MPU6050 not found!");
        return;
    }

    mpu.setAccelerometerRange(MPU6050_RANGE_8_G);
    mpu.setGyroRange(MPU6050_RANGE_500_DEG);
    mpu.setFilterBandwidth(MPU6050_BAND_21_HZ);

    filter.begin(50); // 50 Hz update rate
}

// Variabili globali o statiche per la validazione
int temp_steps = 0;
uint32_t last_step_time = 0;
const int MIN_STEPS_BEFORE_CONFIRM = 3; // Numero di passi consecutivi per confermare la camminata

void Gyroscope_Task(void* pvParameters) {
    Gyroscope_Init();

    sensors_event_t a, g, temp;
    float filteredZ = 0;
    float noise = 0.1f;
    uint32_t last_step_event = 0;

    while (1) {
        mpu.getEvent(&a, &g, &temp);

        // 1. Aggiorna il filtro Madgwick
        filter.updateIMU(g.gyro.x, g.gyro.y, g.gyro.z, a.acceleration.x, a.acceleration.y, a.acceleration.z);

        // 2. Ottieni orientamento per proiettare l'accelerazione sulla verticale (World Z)
        float r = filter.getRollRadians();
        float p = filter.getPitchRadians();
        float worldZ = a.acceleration.x * (-sin(p)) + 
                       a.acceleration.y * (sin(r) * cos(p)) + 
                       a.acceleration.z * (cos(r) * cos(p));

        // 3. Filtro passa-alto per isolare la dinamica del movimento
        filteredZ = 0.9f * filteredZ + 0.1f * worldZ;
        float dyn = worldZ - filteredZ;

        // 4. Calcola l'intensità della rotazione (per capire se stai agitando la mano)
        // Somma delle velocità angolari assolute
        float rotationIntensity = fabs(g.gyro.x) + fabs(g.gyro.y) + fabs(g.gyro.z);

        // 5. Adattamento soglia (incrementata a 1.1f per essere meno sensibile ai piccoli gesti)
        noise = 0.995f * noise + 0.005f * fabs(dyn);
        float threshold = noise + 1.2f; 

        uint32_t now = millis();
        bool peak = (dyn > threshold);
        bool cooldown = (now - last_step_event > 300); // Impedisce di contare due volte lo stesso rimbalzo
        
        // Il "trucco": escludiamo picchi se c'è troppa rotazione (gesto della mano brusco)
        bool isNotTooShaky = (rotationIntensity < 3.5f); 

        if (peak && cooldown && isNotTooShaky && fabs(dyn) < 5.0f) {
            uint32_t interval = now - last_step_time;
            
            // Validazione del ritmo umano (tra 300ms e 1000ms tra un passo e l'altro)
            if (interval > 300 && interval < 1000) {
                temp_steps++;
            } else {
                temp_steps = 1; // Ritmo perso, ricomincia il conteggio
            }
            
            last_step_time = now;
            last_step_event = now;

            // Conferma i passi solo se sono almeno 4 consecutivi
            if (temp_steps >= MIN_STEPS_BEFORE_CONFIRM) {
                if (temp_steps == MIN_STEPS_BEFORE_CONFIRM) {
                    stats.total_steps += MIN_STEPS_BEFORE_CONFIRM; // Aggiungi il "pacchetto" iniziale
                } else {
                    stats.total_steps++; // Continua a contare normalmente
                }

                ESP_LOGI("STEP", "Confermato! Totale: %d | dyn: %.2f", stats.total_steps, dyn);
            }
        }

        // Check Movement for Display Timeout
                if (rotationIntensity > 1.5f) { 
            lastInteractionTime = millis(); 
            
            if (!isDisplayOn) {
                digitalWrite(TFT_BL, HIGH); // Riaccende i LED di retroilluminazione (LED ON)
                isDisplayOn = true;
                ESP_LOGI("Gyro", "Movimento rilevato! TFT Backlight ON");
            }
        }

        vTaskDelay(pdMS_TO_TICKS(20)); // 50Hz
    }
}