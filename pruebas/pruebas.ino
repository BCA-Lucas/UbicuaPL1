#include "DHT.h"           // DHT11
#include <Wire.h>          // Para I2C (BMP280 y BH1750)
#include <Adafruit_Sensor.h> // Para BMP280
#include <Adafruit_BMP280.h> // BMP280
#include <BH1750.h>        // BH1750

// --- CONFIGURACIÓN DE PINES ---
// NOTA: Todos los #define deben ir aquí arriba para evitar errores de ámbito.

// DHT11 (Temperatura y Humedad)
#define DHTPIN 5
#define DHTTYPE DHT11
DHT dht(DHTPIN, DHTTYPE);

// KY-037 (Micrófono/Sonido)
#define MIC_AOUT_PIN 32   // Analógico (Amplitud) - ADC1 seguro
#define MIC_DOUT_PIN 39   // Digital (Umbral) - ADC1 seguro

// YL-83 (Lluvia)
#define RAIN_DOUT_PIN 17  // Digital (Detección)

// ML8511 (UV)
#define UVOUT_PIN 34 
const float VREF = 3.3; 

// BMP280 y BH1750 (I2C Compartido)
#define I2C_SDA_PIN 22    // Pin SDA
#define I2C_SCL_PIN 23    // Pin SCL
Adafruit_BMP280 bmp; 
BH1750 lightMeter;   


// --- FUNCIONES MODULARES DE LECTURA ---
// (El resto de las funciones de lectura van aquí, sin cambios)

bool leer_dht11() {
  float h = dht.readHumidity();
  float t = dht.readTemperature();
  if (isnan(h) || isnan(t)) {
    Serial.println("❌ [DHT11] Error al leer del sensor DHT11!");
    return false;
  }
  Serial.print("  [DHT11] Humedad: "); Serial.print(h); Serial.print(" %\t| Temp: "); Serial.print(t); Serial.println(" °C");
  return true;
}

void leer_ky037() {
  int analog_value = analogRead(MIC_AOUT_PIN); 
  int digital_value = digitalRead(MIC_DOUT_PIN);
  Serial.print("  [KY-037] Analógico (Ruido): "); Serial.print(analog_value);
  Serial.print(" (0-4095)\t| Digital (Umbral): ");
  if (digital_value == HIGH) {
    Serial.println("RUIDO DETECTADO (HIGH) 🔊");
  } else {
    Serial.println("Silencio (LOW) 🤫");
  }
}

void leer_yl83() {
  int digital_value = digitalRead(RAIN_DOUT_PIN);
  Serial.print("  [YL-83] Detección de Lluvia: ");
  if (digital_value == LOW) {
    Serial.println("¡LLUVIA DETECTADA! (LOW) 💧");
  } else {
    Serial.println("Superficie seca (HIGH) ☀️");
  }
}

float leer_ml8511() {
  int uv_analog = analogRead(UVOUT_PIN);
  float uv_voltage = (float)uv_analog * (VREF / 4095.0); 
  float uv_index = (uv_voltage - 1.0) * (10.0 / (2.8 - 1.0));
  if (uv_index < 0) {
    uv_index = 0;
  }
  Serial.print("  [ML8511] Voltaje UV: "); Serial.print(uv_voltage, 3);
  Serial.print(" V\t| Índice UV (Aprox.): "); Serial.println(uv_index, 1);
  return uv_index;
}

void leer_bmp280() {
  float temp_bmp = bmp.readTemperature();
  float pressure = bmp.readPressure() / 100.0F; 

  if (isnan(temp_bmp) || isnan(pressure)) {
     Serial.println("❌ [BMP280] Error al leer del sensor BMP280.");
     return;
  }

  Serial.print("  [BMP280] Temp: "); Serial.print(temp_bmp); Serial.print(" °C");
  Serial.print(" | Presión: "); Serial.print(pressure); Serial.print(" hPa");
  Serial.print(" | Altitud: "); Serial.print(bmp.readAltitude(1013.25)); Serial.println(" m"); 
}

void leer_bh1750() {
  float lux = lightMeter.readLightLevel();

  if (lux < 0) {
    Serial.println("❌ [BH1750] Error al leer del sensor de luz.");
    return;
  }

  Serial.print("  [BH1750] Intensidad de Luz: ");
  Serial.print(lux);
  Serial.println(" Lux");
}


// --- LÓGICA PRINCIPAL DEL PROGRAMA ---

void setup() {
  Serial.begin(115200);
  Serial.println("\n*** Inicio del Programa de 6 Sensores ***");
  
  // 1. Inicialización I2C
  Wire.begin(I2C_SDA_PIN, I2C_SCL_PIN); 
  
  // 2. Inicialización BMP280
  if (!bmp.begin(0x76)) { 
    Serial.println("❌ Error al iniciar el BMP280. Revisa la dirección.");
  } else {
    Serial.print("✅ BMP280. I2C en SDA: GPIO"); Serial.print(I2C_SDA_PIN);
    Serial.print(" y SCL: GPIO"); Serial.print(I2C_SCL_PIN); Serial.println("...");
    bmp.setSampling(Adafruit_BMP280::MODE_NORMAL, Adafruit_BMP280::SAMPLING_X2, 
                    Adafruit_BMP280::SAMPLING_X16, Adafruit_BMP280::FILTER_X16, 
                    Adafruit_BMP280::STANDBY_MS_500);
  }

  // 3. Inicialización BH1750
  if (lightMeter.begin(BH1750::CONTINUOUS_HIGH_RES_MODE)) {
    Serial.println("✅ BH1750 inicializado correctamente.");
  } else {
    Serial.println("❌ Error al iniciar el BH1750. Revisa el pin ADDR/AD0.");
  }

  // 4. Inicialización otros sensores
  dht.begin(); Serial.println("Inicializando DHT11 en pin D5...");
  pinMode(MIC_DOUT_PIN, INPUT); analogReadResolution(12); Serial.println("Inicializando KY-037. A0 en GPIO32 y D0 en GPIO39..."); 
  
  // Pin YL-83 inicializado
  pinMode(RAIN_DOUT_PIN, INPUT); Serial.println("Inicializando YL-83. D0 en GPIO17...");
  
  analogReadResolution(12); Serial.println("Inicializando ML8511 en pin GPIO34..."); 

  delay(2000); 
}

void loop() {
  Serial.println("\n--- Nueva Lectura de Sensores ---");
  
  // Llamada a todas las funciones modulares
  leer_dht11();
  leer_ky037();
  leer_yl83(); 
  leer_ml8511(); 
  leer_bmp280(); 
  leer_bh1750(); 
  
  Serial.println("---------------------------------");
  delay(5000); // Espera 5 segundos antes de la siguiente lectura
}