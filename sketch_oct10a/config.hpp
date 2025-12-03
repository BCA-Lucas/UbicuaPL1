#ifndef CONFIG_HPP
#define CONFIG_HPP

#include <Arduino.h>

// ========================================================
// === Pines I2C y Sensores ===
// ========================================================

// Bus I2C 1 (BMP280)
extern const int BMP_SDA;
extern const int BMP_SCL;

// Bus I2C 2 (OLED) <-- PINES AÑADIDOS PARA RESOLVER EL ERROR
extern const int OLED_SDA;
extern const int OLED_SCL;

// Pines Analógicos y Digitales
extern const int DHTPIN;
extern const int PIN_LLU_D;
extern const int PIN_LLU_A;
extern const int PIN_SON_A;
extern const int PIN_SON_D;
extern const int PIN_UV_A;
extern const int PIN_LED_RUIDO;

// ========================================================
// === Constantes de Sensores ===
// ========================================================
extern const int UMBRAL_VALOR_ALTO;
extern const float V_REF;
extern const float V_UV_CERO;
extern const float UV_SENSITIVITY;
extern const unsigned long LECTURA_INTERVALO;
extern const int N_SAMPLES;

// ========================================================
// === Identificación del Dispositivo ===
// ========================================================
extern const char* SENSOR_IDS[];
extern const int NUM_SENSORS;
extern const char* SENSOR_TYPE;
extern const char* STREET_ID;

extern const float LATITUDE;
extern const float LONGITUDE;
extern const char* DISTRICT;
extern const char* NEIGHBORHOOD;

// ========================================================
// === WiFi ===
// ========================================================
extern const char* WIFI_SSID;
extern const char* WIFI_PASSWORD;

// ========================================================
// === MQTT ===
// ========================================================
extern const char* MQTT_SERVER;
extern const int MQTT_PORT;
extern const char* MQTT_USER;
extern const char* MQTT_PASS;

extern const char* TOPIC_SUB;
extern const char* TOPIC_PUB;

// === ENDPOINT HTTP PARA ENVIAR DATOS ===
extern const char* HTTP_SERVER;
extern const int HTTP_PORT;
extern const char* HTTP_PATH;


#endif