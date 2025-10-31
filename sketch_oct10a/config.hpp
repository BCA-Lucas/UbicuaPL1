// config.hpp
#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <Arduino.h>

// ========================================================
// --- CONFIGURACIÓN DE PINES (USUARIO) ---
// ========================================================

// === Pines I2C (Bus Principal: BMP280) ===
extern const int BMP_SDA; // 23
extern const int BMP_SCL; // 22

// === Pines I2C (Bus Secundario: BH1750) ===
extern const int BH_SDA; // 32
extern const int BH_SCL; // 35

// === Pines Analógicos y Digitales ===
extern const int DHTPIN;        // DHT11 (GPIO 5)
extern const int PIN_LLU_D;     // YL-83 Lluvia Digital (15)
extern const int PIN_LLU_A;     // YL-83 Lluvia Analógica (19)
extern const int PIN_SON_A;     // KY-037 Sonido Analógico (21)
extern const int PIN_SON_D;     // KY-037 Sonido Digital (36)
extern const int PIN_UV_A;      // ML8511 UV Analógico (34)
extern const int PIN_LED_RUIDO; // LED indicador de ruido (2)

#define DHTTYPE DHT11

// ========================================================
// --- UMBRALES, CONSTANTES y COMUNICACIONES ---
// ========================================================
extern const int UMBRAL_VALOR_ALTO; 
extern const float V_REF; 
extern const float V_UV_CERO; 
extern const float UV_SENSITIVITY; 
extern const unsigned long LECTURA_INTERVALO; 
extern const int N_SAMPLES; 

extern const char* SENSOR_ID;
extern const char* SENSOR_TYPE;
extern const char* STREET_ID;
extern const float LATITUDE;
extern const float LONGITUDE;
extern const char* DISTRICT;
extern const char* NEIGHBORHOOD;

extern const char* WIFI_SSID;
extern const char* WIFI_PASSWORD;
extern const char* MQTT_SERVER;
extern const int MQTT_PORT;
extern const char* MQTT_USER; 
extern const char* MQTT_PASS; 
extern const char* TOPIC_SUB;
extern const char* TOPIC_PUB;

#endif
