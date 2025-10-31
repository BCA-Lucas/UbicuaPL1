#include "Comunicaciones.hpp"
#include "config.hpp"

#include <WiFi.h>
#include <PubSubClient.h>
#include <time.h>
#include <Arduino.h>

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600; 
const int daylightOffset_sec = 3600;

WiFiClient espClient;
PubSubClient client(espClient);

void CommsManager::callback(char* topic, byte* message, unsigned int length) {
	Serial.print("Mensaje recibido en [");
	Serial.print(topic);
	Serial.print("]: ");
	for (unsigned int i = 0; i < length; i++) {
		Serial.print((char)message[i]);
	}
	Serial.println();
}

void CommsManager::setup_wifi() {
	delay(10);
	Serial.println();
	Serial.print("Conectando a ");
	Serial.println(WIFI_SSID);

	WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

	while (WiFi.status() != WL_CONNECTED) {
		delay(500);
		Serial.print(".");
	}

	Serial.println("\nWiFi conectado");
	Serial.print("Dirección IP: ");
	Serial.println(WiFi.localIP());
}

void CommsManager::reconnect_mqtt() {
	while (!client.connected()) {
		Serial.print("Intentando conexión MQTT...");
		if (client.connect("ESP32Client", MQTT_USER, MQTT_PASS)) {
			Serial.println("conectado");
			client.subscribe(TOPIC_SUB);
		} else {
			Serial.print("falló, rc=");
			Serial.print(client.state());
			Serial.println(" intentando de nuevo en 5 segundos");
			delay(5000);
		}
	}
}

void CommsManager::inicializarNTP() {
	Serial.println("-> PASO 1.1: Configurando NTP...");
	configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
	
	struct tm timeinfo;

	if (!getLocalTime(&timeinfo, 10000)) {
		Serial.println("-> PASO 1.2: ❌ No se pudo obtener la hora del servidor NTP. El timestamp será estático.");
	} else {
		Serial.println("-> PASO 1.2: ✅ Hora sincronizada.");
	}
}

String CommsManager::obtenerTiempoISO8601() {
	struct tm timeinfo;
	if (!getLocalTime(&timeinfo)) {
		return ""; 
	}
	char timeString[25]; 
	strftime(timeString, sizeof(timeString), "%Y-%m-%dT%H:%M:%SZ", &timeinfo);
	return String(timeString);
}

void CommsManager::inicializarComunicaciones() {
	setup_wifi();
	client.setServer(MQTT_SERVER, MQTT_PORT);
	client.setCallback(callback);
	
	if (WiFi.status() == WL_CONNECTED) {
		inicializarNTP();
	}
}

void CommsManager::mantenerConexion() {
	if (!client.connected()) {
		Serial.println("🔴 Cliente MQTT desconectado. Intentando reconexión...");
		reconnect_mqtt();
	}
	client.loop(); 
}

bool CommsManager::publicar(const char* topic, const String& payload) {
	return client.publish(topic, payload.c_str());
}