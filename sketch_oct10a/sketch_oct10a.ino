#include "config.hpp"
#include "Sensores.hpp"
#include "Comunicaciones.hpp"
#include "StatusNotifier.hpp" // <-- ¡Inclusión de la nueva clase!

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
  commsManager.mantenerConexion();
  
  // Verifica el temporizador y APAGA el LED si el tiempo de parpadeo ha terminado.
  // La información de la pantalla se mantiene visible.
  statusNotifier.manejarParpadeo(); 
  
  unsigned long now = millis();
  
  if (now - lastPublish >= LECTURA_INTERVALO) {
    lastPublish = now;

    sensorManager.leerTodos(currentData);
    
    String jsonPayload = sensorManager.generarJSON(currentData);

    if (commsManager.publicar(TOPIC_PUB, jsonPayload)) {
      Serial.println("✅ Publicado con éxito en MQTT.");
    } else {
      Serial.println("❌ Error al publicar en MQTT.");
    }

    Serial.println("========== DATOS ENVIADOS (JSON) ==========");
    Serial.println(jsonPayload);
    Serial.println("-------------------------------------------");
  }

  delay(1); 
}