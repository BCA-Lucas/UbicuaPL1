// Sensores.cpp
#include "Sensores.hpp"
#include "config.hpp"

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <DHT.h>
#include <DHT_U.h>
#include <BH1750.h> 

// Instancias de Sensores
Adafruit_BMP280 bmp;
DHT dht(DHTPIN, DHTTYPE);
BH1750 lightMeter; 
// Usamos 'extern' para usar la instancia global definida por la librería Wire (soluciona el error de "multiple definition").
extern TwoWire Wire1; 

// ========================================================
// --- FUNCIONES AUXILIARES PRIVADAS ---
// ========================================================
int SensorManager::promedioAnalogico(int pin) {
    long suma = 0;
    for (int i = 0; i < N_SAMPLES; i++) {
        suma += analogRead(pin);
        delay(2);
    }
    return suma / N_SAMPLES;
}

float SensorManager::getUVIndex(float uvVoltage) {
    float uvIntensity = (uvVoltage - V_UV_CERO) * UV_SENSITIVITY;
    if (uvIntensity < 0) uvIntensity = 0;
    float uvIndex = uvIntensity / 2.0; 
    return uvIndex;
}

// ========================================================
// --- MÉTODOS PÚBLICOS ---
// ========================================================
void SensorManager::inicializarSensores() {
    // --- Configuración de Pines ---
    pinMode(PIN_LLU_D, INPUT);
    pinMode(PIN_SON_D, INPUT);
    pinMode(PIN_LED_RUIDO, OUTPUT);

    // --- Configuración ADC (Analógico) ---
    analogReadResolution(12);
    analogSetPinAttenuation(PIN_LLU_A, ADC_11db);
    analogSetPinAttenuation(PIN_SON_A, ADC_11db);
    analogSetPinAttenuation(PIN_UV_A, ADC_11db);

    // --- Inicializar I2C Bus PRIMARIO (BMP280) en 23/22 ---
    Serial.print("Iniciando Bus I2C Primario (BMP280) en SDA:");
    Serial.print(BMP_SDA); Serial.print("/SCL:"); Serial.println(BMP_SCL);
    Wire.begin(BMP_SDA, BMP_SCL); 

    // --- Inicializar I2C Bus SECUNDARIO (BH1750) en 32/35 ---
    Serial.print("Iniciando Bus I2C Secundario (BH1750) en SDA:");
    Serial.print(BH_SDA); Serial.print("/SCL:"); Serial.println(BH_SCL);
    Wire1.begin(BH_SDA, BH_SCL); 

    // --- Inicializar BMP280 (usa Wire) ---
    Serial.println("Iniciando sensor BMP280...");
    // Usamos la sintaxis antigua, confiando en que el sensor usará el bus 'Wire' reasignado.
    if (!bmp.begin(0x76)) { 
        Serial.println("¡No se encontró un BMP280 válido!");
    } else {
        bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,
                         Adafruit_BMP280::SAMPLING_X2,
                         Adafruit_BMP280::SAMPLING_X16,
                         Adafruit_BMP280::FILTER_X16,
                         Adafruit_BMP280::STANDBY_MS_500);
    }

    // --- Inicializar BH1750 (usa Wire1) ---
    Serial.println("Iniciando sensor BH1750...");
    // Pasamos explícitamente el bus &Wire1 al BH1750.
    if (!lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE, 0x23, &Wire1)) {
      Serial.println("¡Error al inicializar BH1750!");
    }

    // --- Inicializar DHT11 (Pin 1) ---
    Serial.print("Iniciando sensor DHT11 en GPIO: "); Serial.println(DHTPIN);
    dht.begin();
}


void SensorManager::leerTodos(SensorData& data) {
    // Lectura de Sensores I2C
    data.luminosity_lux = lightMeter.readLightLevel(); 
    
    float temp_bmp = bmp.readTemperature();
    float presion_bmp = bmp.readPressure() / 100.0;
    
    // --- Lógica de Corrección NAN/Error para BMP280 ---
    if (isnan(presion_bmp) || presion_bmp < 500.0 || presion_bmp > 1100.0) {
        data.atmospheric_pressure_hpa = 0.0; 
        data.temperature_celsius = dht.readTemperature(); 
    } else {
        data.atmospheric_pressure_hpa = presion_bmp;
        data.temperature_celsius = temp_bmp; 
    }
    // --------------------------------------------------

    // Lectura de Sensores DHT11
    float humedad = dht.readHumidity();
    if (isnan(humedad)) {
        data.humidity_percent = 0.0;
    } else {
        data.humidity_percent = humedad;
    }

    // Lectura de Sensor UV (ML8511)
    int uvAnalogico = promedioAnalogico(PIN_UV_A);
    float uvVoltaje = (uvAnalogico * V_REF) / 4095.0;
    data.uv_index = getUVIndex(uvVoltaje);

    // Lectura de Sonido (KY-037)
    data.noise_analog = promedioAnalogico(PIN_SON_A);

    // Lógica LED de Ruido
    if (data.noise_analog > UMBRAL_VALOR_ALTO) { 
        digitalWrite(PIN_LED_RUIDO, HIGH);
    } else {
        digitalWrite(PIN_LED_RUIDO, LOW);
    }

    // Campos JSON que requieren valores derivados o placeholders
    data.air_quality_index = map(data.noise_analog, 0, 4095, 0, 500); 
    data.wind_speed_kmh = 0.0; 
    data.wind_direction_degrees = 0; 
}

// Generación manual de JSON
String SensorManager::generarJSON(const SensorData& data) {
    String jsonString = "{";

    // Nivel superior
    jsonString += "\"sensor_id\":\"" + String(SENSOR_ID) + "\",";
    jsonString += "\"sensor_type\":\"" + String(SENSOR_TYPE) + "\",";
    jsonString += "\"street_id\":\"" + String(STREET_ID) + "\",";
    jsonString += "\"timestamp\":\"2025-10-15T12:34:56.789Z\",";

    // Objeto Location
    jsonString += "\"location\":{";
    jsonString += "\"latitude\":" + String(LATITUDE, 6) + ",";
    jsonString += "\"longitude\":" + String(LONGITUDE, 6) + ",";
    jsonString += "\"district\":\"" + String(DISTRICT) + "\",";
    jsonString += "\"neighborhood\":\"" + String(NEIGHBORHOOD) + "\"";
    jsonString += "},";

    // Objeto Data (Datos específicos de la estación meteorológica)
    jsonString += "\"data\":{";
    jsonString += "\"temperature_celsius\":" + String(data.temperature_celsius, 2) + ",";
    jsonString += "\"humidity_percent\":" + String(data.humidity_percent, 2) + ",";
    jsonString += "\"air_quality_index\":" + String(data.air_quality_index) + ",";
    jsonString += "\"wind_speed_kmh\":" + String(data.wind_speed_kmh, 1) + ",";
    jsonString += "\"wind_direction_degrees\":" + String(data.wind_direction_degrees) + ",";
    jsonString += "\"atmospheric_pressure_hpa\":" + String(data.atmospheric_pressure_hpa, 2) + ","; 
    jsonString += "\"uv_index\":" + String(data.uv_index, 1) + ",";
    jsonString += "\"luminosity_lux\":" + String(data.luminosity_lux, 2);
    jsonString += "}";

    jsonString += "}";
    return jsonString;
}