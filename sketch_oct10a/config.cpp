// config.cpp
#include "config.hpp"
#include <Arduino.h>

// ========================================================
// --- CONFIGURACIÓN DE PINES (VALORES DEL USUARIO) ---
// ========================================================

// === Pines I2C (Bus 1: BMP280) ===
const int BMP_SDA = 23; 
const int BMP_SCL = 22; 

// === Pines I2C (Bus 2: BH1750) ===
const int BH_SDA = 32; 
const int BH_SCL = 35; 

// === Pines Analógicos y Digitales ===
const int DHTPIN = 5;
const int PIN_LLU_D = 15; 
const int PIN_LLU_A = 19;
const int PIN_SON_A = 21;
const int PIN_SON_D = 36;
const int PIN_UV_A = 34;
const int PIN_LED_RUIDO = 2; 

// ========================================================
// --- UMBRALES, CONSTANTES y TIEMPOS ---
// ========================================================
const int UMBRAL_VALOR_ALTO = 1500; 

// Constantes para el ML8511 (Sensor UV)
const float V_REF = 3.3; 
const float V_UV_CERO = 1.0; 
const float UV_SENSITIVITY = 15.0; 

// Frecuencia de lectura y publicación (en milisegundos)
const unsigned long LECTURA_INTERVALO = 10000; 
const int N_SAMPLES = 8; 

// ========================================================
// --- CONFIGURACIÓN DE LA ESTACIÓN (JSON) ---
// ========================================================
const char* SENSOR_ID = "WS_001";
const char* SENSOR_TYPE = "weather";
const char* STREET_ID = "ST_001";
const float LATITUDE = 40.416775;
const float LONGITUDE = -3.703790;
const char* DISTRICT = "Centro";
const char* NEIGHBORHOOD = "Sol";

// ========================================================
// --- CONFIGURACIÓN DE COMUNICACIONES (MQTT/WIFI) ---
// ========================================================
// Configuración de Hotspot
const char* WIFI_SSID = "Orden"; 
const char* WIFI_PASSWORD = "rT123456";

// Configuración MQTT
const char* MQTT_SERVER = "172.20.10.3"; 
const int MQTT_PORT = 1883;
const char* MQTT_USER = ""; 
const char* MQTT_PASS = ""; 

const char* TOPIC_SUB = "esp32/prueba/comandos"; 
const char* TOPIC_PUB = "esp32/prueba/data/WS_001";
