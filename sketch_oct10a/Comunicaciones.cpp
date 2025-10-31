// Comunicaciones.cpp
#include "Comunicaciones.hpp"
#include "config.hpp"

#include <WiFi.h>
#include <PubSubClient.h>

// Instancias de Objetos
WiFiClient espClient;
PubSubClient client(espClient);

// ========================================================
// --- FUNCIONES AUXILIARES PRIVADAS ---
// ========================================================

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

// ========================================================
// --- MÉTODOS PÚBLICOS ---
// ========================================================
void CommsManager::inicializarComunicaciones() {
    setup_wifi();
    client.setServer(MQTT_SERVER, MQTT_PORT);
    client.setCallback(callback);
}

void CommsManager::mantenerConexion() {
    // Si la conexión se cae, intenta reconectar
    if (!client.connected()) {
        Serial.println("🔴 Cliente MQTT desconectado. Intentando reconexión...");
        reconnect_mqtt();
    }
    // Procesa el tráfico de red y keep-alive
    client.loop(); 
}

bool CommsManager::publicar(const char* topic, const String& payload) {
    return client.publish(topic, payload.c_str());
}