#include "config.hpp"
#include "Sensores.hpp"
#include "Comunicaciones.hpp"

SensorManager sensorManager;
CommsManager commsManager;
SensorData currentData;

unsigned long lastPublish = 0; 
#include "LedNotifier.hpp"

LedNotifier ledNotifier;

void setup() {
	Serial.begin(19200);
	delay(1000);

	Serial.println("=========================================");
	Serial.println("	INICIANDO ESTACIÓN AMBIENTAL ESP32 	");
	Serial.println("=========================================");

	sensorManager.inicializarSensores();
	commsManager.inicializarComunicaciones();
	ledNotifier.inicializar(PIN_LED_RUIDO);

	Serial.println("Sistema iniciado y conectado.\n");
}

void loop() {
  commsManager.mantenerConexion();
  
  // Llama a esta función en cada ciclo para verificar si el temporizador de 
  // recepción del LED ha expirado y apagarlo, creando el efecto de parpadeo.
  ledNotifier.manejarParpadeo(); 
  
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

  delay(1000); 
}