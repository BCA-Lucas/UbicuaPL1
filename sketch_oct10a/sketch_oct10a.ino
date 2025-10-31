// EstacionAmbiental.ino
#include "config.hpp"
#include "Sensores.hpp"
#include "Comunicaciones.hpp"

// Instancias de los managers
SensorManager sensorManager;
CommsManager commsManager;
SensorData currentData;

unsigned long lastPublish = 0; 

// ========================================================
// --- SETUP ---
// ========================================================
void setup() {
    Serial.begin(19200);
    delay(1000);

    Serial.println("=========================================");
    Serial.println("   INICIANDO ESTACIÓN AMBIENTAL ESP32    ");
    Serial.println("=========================================");

    // Inicializar sensores y comunicaciones
    sensorManager.inicializarSensores();
    commsManager.inicializarComunicaciones();

    Serial.println("Sistema iniciado y conectado.\n");
}

// ========================================================
// --- LOOP PRINCIPAL ---
// ========================================================
void loop() {
    // Mantiene la conexión MQTT, procesa keep-alive y mensajes entrantes
    commsManager.mantenerConexion();
    
    unsigned long now = millis();
    
    // Intervalo de lectura y publicación
    if (now - lastPublish >= LECTURA_INTERVALO) {
        lastPublish = now;

        // 1. Leer Sensores
        sensorManager.leerTodos(currentData);
        
        // 2. Generar JSON
        String jsonPayload = sensorManager.generarJSON(currentData);

        // 3. Publicar por MQTT
        if (commsManager.publicar(TOPIC_PUB, jsonPayload)) {
            Serial.println("✅ Publicado con éxito en MQTT.");
        } else {
            Serial.println("❌ Error al publicar en MQTT.");
        }

        // --- IMPRESIÓN DE DATOS POR SERIAL (Debug) ---
        Serial.println("========== DATOS ENVIADOS (JSON) ==========");
        Serial.println(jsonPayload);
        Serial.println("-------------------------------------------");
    }

    // Delay mínimo para evitar bloqueo y permitir que el client.loop() sea llamado rápidamente
    delay(1); 
}