#include "config.hpp"
#include "Sensores.hpp"
#include "Comunicaciones.hpp"

SensorManager sensorManager;
CommsManager commsManager;
SensorData currentData;

unsigned long lastPublish = 0; 

void setup() {
	Serial.begin(19200);
	delay(1000);

	Serial.println("=========================================");
	Serial.println("	INICIANDO ESTACIÓN AMBIENTAL ESP32 	");
	Serial.println("=========================================");

	sensorManager.inicializarSensores();
	commsManager.inicializarComunicaciones();

	Serial.println("Sistema iniciado y conectado.\n");
}

void loop() {
	commsManager.mantenerConexion();
	
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