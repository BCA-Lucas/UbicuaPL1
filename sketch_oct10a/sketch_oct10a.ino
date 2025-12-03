#include "config.hpp"
#include "Sensores.hpp"
#include "Comunicaciones.hpp"
#include "StatusNotifier.hpp"

SensorManager sensorManager;
CommsManager commsManager;

// DECLARACIÓN GLOBAL DE LA NUEVA CLASE
StatusNotifier statusNotifier; 

SensorData currentData;

unsigned long lastPublish = 0; 

void setup() {
  Serial.begin(19200);
  delay(1000);

  Serial.println("=========================================");
  Serial.println("  INICIANDO ESTACIÓN AMBIENTAL ESP32   ");
  Serial.println("=========================================");

  // Inicializa el pin del LED (GPIO 2) y la pantalla OLED
  statusNotifier.inicializar(PIN_LED_RUIDO); 

  sensorManager.inicializarSensores();
  commsManager.inicializarComunicaciones();

  Serial.println("Sistema iniciado y conectado.\n");
}

void loop() {
    // Mantener conexión MQTT y procesar callbacks
    commsManager.mantenerConexion();

    // Manejar parpadeo del LED
    statusNotifier.manejarParpadeo(); 

    unsigned long now = millis();

    if (now - lastPublish >= LECTURA_INTERVALO) {
        lastPublish = now;

        // Leer todos los sensores
        sensorManager.leerTodos(currentData);

        // Generar JSON
        String jsonPayload = sensorManager.generarJSON(currentData);

        // Publicar por MQTT
        if (commsManager.publicar(TOPIC_PUB, jsonPayload)) {
            Serial.println("✅ Publicado en MQTT");
        } else {
            Serial.println("❌ Error publicando en MQTT");
        }

        // Enviar JSON por HTTP POST
        commsManager.enviarHTTP(jsonPayload);

        // Mostrar JSON en serial
        Serial.println("========== DATOS ENVIADOS ==========");
        Serial.println(jsonPayload);
        Serial.println("-----------------------------------");
    }

    delay(1); // Pequeño delay para estabilidad
}
