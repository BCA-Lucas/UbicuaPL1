#include "config.hpp"

// ========================================================
// === Pines I2C y Sensores ===
// ========================================================

// Bus I2C 1 (BMP280) - Bus Secundario (Wire1)
const int BMP_SDA = 18; 
const int BMP_SCL = 4;  

// Bus I2C 2 (OLED) - Bus Principal (Wire)
const int OLED_SDA = 26; // OLED SDA
const int OLED_SCL = 27; // OLED SCL

const int DHTPIN = 5; 
const int PIN_LLU_D = 15;
const int PIN_LLU_A = 19;
const int PIN_SON_A = 35;
const int PIN_SON_D = 36;
const int PIN_UV_A = 34;
const int PIN_LED_RUIDO = 2; 

// ========================================================
// === Constantes de Sensores ===
// ========================================================
const int UMBRAL_VALOR_ALTO = 1500;
const float V_REF = 3.3;
const float V_UV_CERO = 1.0;
const float UV_SENSITIVITY = 15.0;

const unsigned long LECTURA_INTERVALO = 10000;
const int N_SAMPLES = 8;

// ========================================================
// === Identificación del Dispositivo ===
// ========================================================
const char* SENSOR_IDS[] = {"BMP280", "YL-83", "KY-037", "DHT11", "ML8511"};
const int NUM_SENSORS = sizeof(SENSOR_IDS) / sizeof(SENSOR_IDS[0]);
const char* SENSOR_TYPE = "weather";
const char* STREET_ID = "ST_2091";

const float LATITUDE = 40.4432621;
const float LONGITUDE = -3.61235565;
const char* DISTRICT = "San Blas-Canillejas";
const char* NEIGHBORHOOD = "San Blas-Canillejas";

// ========================================================
// === WiFi ===
// ========================================================
const char* WIFI_SSID = "cubicua";
const char* WIFI_PASSWORD = "";

// ========================================================
// === MQTT ===
// ========================================================
const char* MQTT_SERVER = "192.168.1.131";
const int MQTT_PORT = 1883;
const char* MQTT_USER = "";
const char* MQTT_PASS = "";

const char* TOPIC_SUB = "esp32/prueba/comandos";
const char* TOPIC_PUB = "esp32/prueba/data/WS_001";