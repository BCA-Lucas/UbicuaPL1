#include "Sensores.hpp"
#include "config.hpp"
#include "Comunicaciones.hpp"
#include <Arduino.h>

#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <DHT.h>
#include <BH1750.h>

#define DHTTYPE DHT11

Adafruit_BMP280 bmp;
DHT dht(DHTPIN, DHTTYPE);
BH1750 lightMeter;
static TwoWire Wire1Bus(1); // Renombrado para evitar conflicto con librería Wire.cpp

extern CommsManager commsManager;

int SensorManager::promedioAnalogico(int pin) {
    long suma = 0;
    for (int i = 0; i < N_SAMPLES; i++) {
        suma += analogRead(pin);
        delay(2);
    }
    return suma / N_SAMPLES;
}

float SensorManager::getUVIndex(float uvVoltage) {
    float uvIndex = (uvVoltage - 1.0) / 0.1;
    return uvIndex < 0 ? 0 : uvIndex;
}

void SensorManager::inicializarSensores() {
    pinMode(PIN_LLU_D, INPUT);
    pinMode(PIN_SON_D, INPUT);
    pinMode(PIN_LED_RUIDO, OUTPUT);

    analogReadResolution(12);
    analogSetPinAttenuation(PIN_LLU_A, ADC_11db);
    analogSetPinAttenuation(PIN_SON_A, ADC_11db);
    analogSetPinAttenuation(PIN_UV_A, ADC_11db);

    Wire.begin(BMP_SDA, BMP_SCL);
    Wire1Bus.begin(BH_SDA, BH_SCL);

    if (!bmp.begin(0x76)) {
        Serial.println("❌ BMP280 no detectado");
    } else {
        bmp.setSampling(Adafruit_BMP280::MODE_NORMAL,
                        Adafruit_BMP280::SAMPLING_X2,
                        Adafruit_BMP280::SAMPLING_X16,
                        Adafruit_BMP280::FILTER_X16,
                        Adafruit_BMP280::STANDBY_MS_500);
    }

    if (!lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE, 0x23, &Wire1Bus)) {
        Serial.println("❌ Error al inicializar BH1750");
    }

    dht.begin();
}

void SensorManager::leerTodos(SensorData& data) {
    data.luminosity_lux = lightMeter.readLightLevel();

    float temp_bmp = bmp.readTemperature();
    float presion_bmp = bmp.readPressure() / 100.0;

    if (isnan(presion_bmp) || presion_bmp < 500.0 || presion_bmp > 1100.0) {
        data.atmospheric_pressure_hpa = 0.0;
        data.temperature_celsius = dht.readTemperature();
    } else {
        data.atmospheric_pressure_hpa = presion_bmp;
        data.temperature_celsius = temp_bmp;
    }

    data.humidity_percent = dht.readHumidity();
    int uvAnalogico = promedioAnalogico(PIN_UV_A);
    float uvVoltaje = (uvAnalogico * V_REF) / 4095.0;
    data.uv_index = getUVIndex(uvVoltaje);

    data.noise_analog = promedioAnalogico(PIN_SON_A);
    digitalWrite(PIN_LED_RUIDO, data.noise_analog > UMBRAL_VALOR_ALTO ? HIGH : LOW);
    data.rain_detected = digitalRead(PIN_LLU_D);
    data.air_quality_index = map(data.noise_analog, 0, 4095, 0, 500);
    data.wind_speed_kmh = 0.0;
    data.wind_direction_degrees = 0;
}

String SensorManager::generarJSON(const SensorData& data) {
    String json = "{";
    String timestamp = commsManager.obtenerTiempoISO8601();
    if (timestamp.isEmpty()) timestamp = "2025-10-15T12:34:56Z";

    json += "\"sensor_id\":\"" + String(SENSOR_ID) + "\",";
    json += "\"sensor_type\":\"" + String(SENSOR_TYPE) + "\",";
    json += "\"street_id\":\"" + String(STREET_ID) + "\",";
    json += "\"timestamp\":\"" + timestamp + "\",";
    json += "\"location\":{";
    json += "\"latitude\":" + String(LATITUDE, 6) + ",";
    json += "\"longitude\":" + String(LONGITUDE, 6) + ",";
    json += "\"district\":\"" + String(DISTRICT) + "\",";
    json += "\"neighborhood\":\"" + String(NEIGHBORHOOD) + "\"";
    json += "},";
    json += "\"data\":{";
    json += "\"temperature_celsius\":" + String(data.temperature_celsius, 2) + ",";
    json += "\"humidity_percent\":" + String(data.humidity_percent, 2) + ",";
    json += "\"air_quality_index\":" + String(data.air_quality_index) + ",";
    json += "\"atmospheric_pressure_hpa\":" + String(data.atmospheric_pressure_hpa, 2) + ",";
    json += "\"uv_index\":" + String(data.uv_index, 1) + ",";
    json += "\"luminosity_lux\":" + String(data.luminosity_lux, 2) + ",";
    json += "\"rain_detected\":" + String(data.rain_detected) + ",";
    json += "\"noise_analog\":" + String(data.noise_analog);
    json += "}}";
    return json;
}
